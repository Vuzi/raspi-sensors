#include "wrapper_sensor.h"

#ifdef USE_GPIO
#include "DHT22.h"
#include "PIR.h"
#endif
#include "TSL2561.h"
#include "BMP180.h"

using namespace v8;

const struct sensor::sensorConf conf[] = {
    #ifdef USE_GPIO
    {
        type    : "DHT22",
        bus     : sensor::GPIO,
        factory : sensor::DHT22_sensor::create
    },
    {
        type    : "PIR",
        bus     : sensor::GPIO,
        factory : sensor::PIR_sensor::create
    },
    #endif
    {
        type    : "TSL2561",
        bus     : sensor::I2C,
        factory : sensor::TSL2561_sensor::create
    },
    {
        type    : "BMP180",
        bus     : sensor::I2C,
        factory : sensor::BMP180_sensor::create
    }
};

Persistent<Function> SensorWrapper::constructor;

SensorWrapper::SensorWrapper(sensor::sensor* s) : _s(s) {}

SensorWrapper::~SensorWrapper() {
    // Free the sensor
    delete _s;
}

sensor::sensor* SensorWrapper::InitSensor(const Local<String>& sensorName, const Local<Object>& sensorConfig) {
    Isolate* isolate = Isolate::GetCurrent();

    const Local<String> t = String::NewFromUtf8(isolate, "type");

    // Check for type
    if(!sensorConfig->Has(t) || !sensorConfig->Get(t)->IsString())
        throw Exception::TypeError(
            String::NewFromUtf8(isolate, "Error : no type specified. All sensors require a valid 'type' property"));

    // Get the sensor name
    String::Utf8Value nameValue(sensorName->ToString());
    const std::string name = std::string(*nameValue);

    // Get the type of sensor
    const Local<Value> jsvalue = sensorConfig->Get(t);
    String::Utf8Value value(jsvalue->ToString());
    const std::string type = std::string(*value);

    const size_t size = std::extent<decltype(conf)>::value;
    
    for(size_t i = 0; i < size; i++) {
        if(conf[i].type == type) {
            const Local<String> prop = String::NewFromUtf8(isolate, (conf[i].bus == sensor::GPIO ? "pin" : "address"));

            // Get the required property
            if(!sensorConfig->Has(prop) || !sensorConfig->Get(prop)->IsNumber()) {
                if(conf[i].bus == sensor::GPIO) {
                    throw Exception::TypeError(
                        String::NewFromUtf8(isolate, "Error : a valid pin property is required for GPIO sensor (number >= 0x0)"));
                } else {
                    throw Exception::TypeError(
                        String::NewFromUtf8(isolate, "Error : a valid address property is required for I2C sensor (number >= 0x0)"));
                }
            }

            Local<Number> propValue = Local<Number>::Cast(sensorConfig->Get(prop));
            sensor::sensor *s = conf[i].factory((int)propValue->NumberValue(), name);

            return s;
        }
    }

    throw Exception::TypeError(
        String::NewFromUtf8(isolate, fmt::format("Error : '{0}' is not a valid sensor type", type).c_str()));

    return nullptr;
}

void SensorWrapper::SendError(sensor::sensor* s, sensor::sensorException& e, Isolate* isolate, Local<Function>& cb) {
    Local<Object> result = Object::New(isolate); 

    Local<String> cause = String::NewFromUtf8(isolate, e.what());
    Local<Number> code = Number::New(isolate, e.code());

    result->Set(String::NewFromUtf8(isolate, "cause"), cause);
    result->Set(String::NewFromUtf8(isolate, "code"), code);

    // Call the callback with the values
    Local<Value> argv[2] = { result, Undefined(isolate) };
    cb->Call(isolate->GetCurrentContext()->Global(), 2, argv);
}

void SensorWrapper::SendResult(sensor::sensor* s, sensor::result& r, Isolate* isolate, Local<Function>& cb) {
    // Get the type of value & the value
    Local<Object> result = Object::New(isolate); 
    
    Local<String> type;
    Local<String> unit;
    Local<String> unit_display;
    Local<Number> value;

    if(r.getType() == sensor::resultType::TEMPERATURE) {
        type = String::NewFromUtf8(isolate, "Temperature");
        unit = String::NewFromUtf8(isolate, "Degree Celsius");
        unit_display = String::NewFromUtf8(isolate, "°C");
        value = Number::New(isolate, r.getValue().f);
    }
    else if(r.getType() == sensor::resultType::HUMIDITY) {
        type = String::NewFromUtf8(isolate, "Humidity");
        unit = String::NewFromUtf8(isolate, "Percent");
        unit_display = String::NewFromUtf8(isolate, "%");
        value = Number::New(isolate, r.getValue().f);
    }
    else if(r.getType() == sensor::resultType::LIGHT) {
        type = String::NewFromUtf8(isolate, "Light");
        unit = String::NewFromUtf8(isolate, "Lux");
        unit_display = String::NewFromUtf8(isolate, "Lux");
        value = Number::New(isolate, r.getValue().i);
    }
    else if(r.getType() == sensor::resultType::PRESSURE) {
        type = String::NewFromUtf8(isolate, "Pressure");
        unit = String::NewFromUtf8(isolate, "Pascal");
        unit_display = String::NewFromUtf8(isolate, "Pa");
        value = Number::New(isolate, r.getValue().f);
    }
    else if(r.getType() == sensor::resultType::DETECTION) {
        type = String::NewFromUtf8(isolate, "Detection");
        unit = String::NewFromUtf8(isolate, "Boolean");
        unit_display = String::NewFromUtf8(isolate, "Boolean");
        value = Number::New(isolate, r.getValue().i);
    }
    else {
        type = String::NewFromUtf8(isolate, "Other");
        unit = String::NewFromUtf8(isolate, "-");
        unit_display = String::NewFromUtf8(isolate, "");
        value = Number::New(isolate, 0);
    }
    
    result->Set(String::NewFromUtf8(isolate, "type"), type);
    result->Set(String::NewFromUtf8(isolate, "unit"), unit);
    result->Set(String::NewFromUtf8(isolate, "unit_display"), unit_display);
    result->Set(String::NewFromUtf8(isolate, "value"), value);
    result->Set(String::NewFromUtf8(isolate, "date"), Date::New(isolate, r.getTimestamp()));
    result->Set(String::NewFromUtf8(isolate, "timestamp"), Number::New(isolate, r.getTimestamp()));
    result->Set(String::NewFromUtf8(isolate, "sensor_name"), String::NewFromUtf8(isolate, s->getName().c_str()));
    result->Set(String::NewFromUtf8(isolate, "sensor_type"), String::NewFromUtf8(isolate, s->getType().c_str()));

    // Call the callback with the values
    Local<Value> argv[2] = { Undefined(isolate), result };
    cb->Call(isolate->GetCurrentContext()->Global(), 2, argv);
}

void SensorWrapper::fetchClear() {
    for (auto handler : schedulers) {
        handler->cancel();
    }

    schedulers.clear();
}

void SensorWrapper::fetch(const FunctionCallbackInfo<Value>& args, bool repeatable) {
    Isolate* isolate = args.GetIsolate();

    if(!args[0]->IsFunction()) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "Error : no callback provided. A callback is required to read result(s) from sensor")));
        return;
    }

    if(repeatable && (!args[1]->IsNumber() || args[1]->NumberValue() < 0)) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "Error : no duration provided. A repeatable reading needs a valid duration")));
        return;
    }

    Local<Function> cb = Local<Function>::Cast(args[0]);
    int value = (repeatable ? args[1]->NumberValue() : 0);

    // Get persitent value of the callback and object, to avoid nodejs deleting it
    Persistent<Function, CopyablePersistentTraits<Function>> callback(isolate, cb);
    Persistent<Object, CopyablePersistentTraits<Object>> sensorWrapperObj(isolate, args.Holder());

    scheduler::scheduler<sensor::sensor*, sensor::resultsOrError>* handler = 
    new scheduler::scheduler<sensor::sensor*, sensor::resultsOrError>(_s,
        [](sensor::sensor* s) {
            return s->getResultsOrError();
        },
        [callback, isolate](sensor::sensor* s, sensor::resultsOrError results) {
            // Local reference of the callback
            Local<Function> cb = Local<Function>::New(isolate, callback);

            if(results.hasError()) {
                sensor::sensorException e = results.getError();
                SendError(s, e, isolate, cb);
            } else {
                for(sensor::result r : results.getResults()) {
                    SendResult(s, r, isolate, cb);
                }
            }
        },
        [callback, sensorWrapperObj]() mutable {
            callback.Reset();
            sensorWrapperObj.Reset();
        }, value, repeatable);
    handler->launch();

    // Also keep track of the sheduler
    if(repeatable)
        schedulers.push_back(handler);
}

void SensorWrapper::Init(Local<Object> exports) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
    tpl->SetClassName(String::NewFromUtf8(isolate, "Sensor"));
    tpl->InstanceTemplate()->SetInternalFieldCount(3);

    // Prototype
    NODE_SET_PROTOTYPE_METHOD(tpl, "fetch", Fetch);
    NODE_SET_PROTOTYPE_METHOD(tpl, "fetchInterval", FetchInterval);
    NODE_SET_PROTOTYPE_METHOD(tpl, "fetchClear", FetchClear);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(String::NewFromUtf8(isolate, "Sensor"), tpl->GetFunction());
}

void SensorWrapper::New(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();

    if (args.IsConstructCall()) {
        if(!args[0]->IsObject()) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Error : no configuration specified. All sensors require a valid configuration")));
            return;
        }

        const Local<Object> conf = args[0]->ToObject();
        const Local<String> name = args[1]->ToString();

        try {
            // Create a sensor
            SensorWrapper* obj = new SensorWrapper(InitSensor(name, conf));
            obj->Wrap(args.This());
            args.GetReturnValue().Set(args.This());
        } catch(Local<Value> &e) {
            // Transfer the exception to node
            isolate->ThrowException(e);
        }
    } else {
        // Invoked as plain function, turn into construct call.
        const int argc = 1;
        Local<Value> argv[argc] = { args[0] };
        Local<Function> cons = Local<Function>::New(isolate, constructor);
        args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void SensorWrapper::Fetch(const FunctionCallbackInfo<Value>& args) {
    SensorWrapper* sensorWrapper = ObjectWrap::Unwrap<SensorWrapper>(args.Holder());
    sensorWrapper->fetch(args);
}

void SensorWrapper::FetchInterval(const FunctionCallbackInfo<Value>& args) {
    SensorWrapper* sensorWrapper = ObjectWrap::Unwrap<SensorWrapper>(args.Holder());
    sensorWrapper->fetch(args, true);
}

void SensorWrapper::FetchClear(const FunctionCallbackInfo<Value>& args) {
    SensorWrapper* sensorWrapper = ObjectWrap::Unwrap<SensorWrapper>(args.Holder());
    sensorWrapper->fetchClear();
}
