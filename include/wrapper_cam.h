/**
 * @file wrapper_sensor.h
 * @brief Wrapper class for using sensors and scheduler from nodejs plugin
 * @author Vuzi
 * @version 0.2
 */

#ifndef SENSOR_WRAPPER_H
#define SENSOR_WRAPPER_H

#include <node.h>
#include <node_object_wrap.h>

#include "format.h"

#include "scheduler.h"
#include "sensor.h"
#include "sensor_result.h"

#include <ctime>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <raspicam/raspicam.h>
#include <raspicam/raspicamtypes.h>

using namespace v8;


/**
 * @class SensorWrapper
 * @brief NodeJS exposed class
 */
class CamWrapper : public node::ObjectWrap {

	public:
		static void Init(Local<Object> exports);

	private:
		explicit CamWrapper(raspicam::RaspiCam* c);
		~CamWrapper();

		// Private method used to generate a sensor and a result
		static raspicam::RaspiCam* InitCam(const Local<String>& sensorName, const Local<Object>& sensorConfig);
		static void SendResult(raspicam::RaspiCam* s, sensor::result& r, Isolate* isolate, Local<Function>& cb);
		static void SendError(raspicam::RaspiCam* s, sensor::sensorException& r, Isolate* isolate, Local<Function>& cb);
		
		void setParameter(const FunctionCallbackInfo<Value>& args);
		void takePicture(const FunctionCallbackInfo<Value>& args);

		// Node exported methods
		static void New(const FunctionCallbackInfo<Value>& args);
		static void SetParameter(const FunctionCallbackInfo<Value>& args);
		static void TakePicture(const FunctionCallbackInfo<Value>& args);

		static Persistent<Function> constructor;
		raspicam::RaspiCam* _c;
};


#endif // SENSOR_WRAPPER_H


