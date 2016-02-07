#include "wrapper_sensor.h"

using namespace v8;

Persistent<Function> CamWrapper::constructor;

CamWrapper::CamWrapper(raspicam::RaspiCam* c) : _c(c) {}

CamWrapper::~CamWrapper() {
    // Free the sensor
    delete _c;
}

raspicam::RaspiCam* CamWrapper::InitCam(const Local<Object>& camConfig) {
    //Isolate* isolate = Isolate::GetCurrent();

    raspicam::RaspiCam camera* = new raspicam::RaspiCam();

    return camera;
}

void CamWrapper::SendError(raspicam::RaspiCam* s, sensor::sensorException& e, Isolate* isolate, Local<Function>& cb) {
    Local<Object> result = Object::New(isolate); 

    Local<String> cause = String::NewFromUtf8(isolate, e.what());
    Local<Number> code = Number::New(isolate, e.code());

    result->Set(String::NewFromUtf8(isolate, "cause"), cause);
    result->Set(String::NewFromUtf8(isolate, "code"), code);

    // Call the callback with the values
    Local<Value> argv[2] = { result, Undefined(isolate) };
    cb->Call(isolate->GetCurrentContext()->Global(), 2, argv);
}

void CamWrapper::SendResult(raspicam::RaspiCam* s, sensor::result& r, Isolate* isolate, Local<Function>& cb) {
    // Get the type of value & the value
    Local<Object> result = Object::New(isolate); 
    
    Local<String> type;
    Local<String> unit;
    Local<String> unit_display;
    Local<Number> value;

    if(r.getType() == sensor::resultType::PHOTO) {
        type = String::NewFromUtf8(isolate, "Photo");
        unit = String::NewFromUtf8(isolate, "-");
        unit_display = String::NewFromUtf8(isolate, "-");
        value = String::NewFromUtf8(isolate, r.getValue().s);
    } else {
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

void CamWrapper::setParameter(const FunctionCallbackInfo<Value>& args) {

}

void CamWrapper::takePicture(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();

    if(!args[0]->IsFunction()) {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "Error : no callback provided. A callback is required to read result(s) from camera")));
        return;
    }

    Local<Function> cb = Local<Function>::Cast(args[0]);

    // Get persitent value of the callback and object, to avoid nodejs deleting it
    Persistent<Function, CopyablePersistentTraits<Function>> callback(isolate, cb);
    Persistent<Object, CopyablePersistentTraits<Object>> sensorCamObj(isolate, args.Holder());

    scheduler::scheduler<raspicam::RaspiCam*, sensor::resultsOrError>* handler = 
    new scheduler::scheduler<raspicam::RaspiCam*, sensor::resultsOrError>(_s,
        [](raspicam::RaspiCam* cam) {
            // Camera opening
            if (!cam->isOpen() && !cam->open()) {
                return resultsOrError(sensorException("Camera could not be opened", sensor::sensorErrorCode::INVALID_VALUE));
            }

            // Wait a while until camera stabilizes
            sleep(3);

            // Grab the camera
            cam->grab();

            // Allocate memory
            unsigned char *data = new unsigned char[cam->getImageTypeSize(raspicam::RASPICAM_FORMAT_RGB)];
            
            // Extract the image in rgb format
            cam->retrieve(data,raspicam::RASPICAM_FORMAT_IGNORE);
            
            // Save as a file
            std::ofstream outFile("raspicam_image.ppm", std::ios::binary);
            outFile << "P6\n" << cam->getWidth() << " " << cam->getHeight() << " 255\n";
            outFile.write ((char*)data, cam->getImageTypeSize(raspicam::RASPICAM_FORMAT_RGB));
            std::cout << "Image saved at raspicam_image.ppm" << std::endl;

            // Prepare results
            sensor::resultValue camValue;
            camValue.s = "raspicam_image.ppm";
            
            sensor::result camResult(resultType::PHOTO, camValue);
            std::list<sensor::result> results;

            results.push_back(camResult);

            return resultsOrError(results);
        },
        [callback, isolate](raspicam::RaspiCam* s, sensor::resultsOrError results) {
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
        [callback, sensorCamObj]() mutable {
            callback.Reset();
            sensorCamObj.Reset();
        }, 0, false);
    handler->launch();
}

void CamWrapper::Init(Local<Object> exports) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
    tpl->SetClassName(String::NewFromUtf8(isolate, "Camera"));
    tpl->InstanceTemplate()->SetInternalFieldCount(3);

    // Prototype
    NODE_SET_PROTOTYPE_METHOD(tpl, "takePicture", TakePicture);
    NODE_SET_PROTOTYPE_METHOD(tpl, "setParameter", SetParameter);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(String::NewFromUtf8(isolate, "Camera"), tpl->GetFunction());
}

void CamWrapper::New(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();

    if (args.IsConstructCall()) {
        if(!args[0]->IsObject()) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Error : no configuration specified")));
            return;
        }

        const Local<Object> conf = args[0]->ToObject();

        try {
            // Create a sensor
            CamWrapper* obj = new CamWrapper(InitCam(conf));
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

void CamWrapper::TakePicture(const FunctionCallbackInfo<Value>& args) {
    CamWrapper* camWrapper = ObjectWrap::Unwrap<CamWrapper>(args.Holder());
    camWrapper->takePicture(args);
}

void CamWrapper::SetParameter(const FunctionCallbackInfo<Value>& args) {
    CamWrapper* camWrapper = ObjectWrap::Unwrap<CamWrapper>(args.Holder());
    camWrapper->setParameter(args);
}
