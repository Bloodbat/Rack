#pragma once
#include <vector>

#include <common.hpp>
#include <engine/Module.hpp>
#include <engine/Cable.hpp>
#include <engine/ParamHandle.hpp>


namespace rack {
namespace engine {


/** Manages Modules and Cables and steps them in time.

Engine contains a read/write mutex that locks when the Engine state is being read or written (manipulated).
Methods that read-lock (stated in their documentation) can be called simultaneously with other read-locking methods.
Methods that write-lock cannot be called simultaneously or recursively with another read-locking or write-locking method.
*/
struct Engine {
	struct Internal;
	Internal* internal;

	Engine();
	~Engine();

	/** Removes all modules and cables.
	Write-locks.
	*/
	void clear();
	INTERNAL void clear_NoLock();
	/** Advances the engine by `frames` frames.
	Only call this method from the primary module.
	Read-locks. Also locks so only one stepBlock() can be called simultaneously or recursively.
	*/
	void stepBlock(int frames);
	/** Module does not need to belong to the Engine.
	However, Engine will unset the primary module when it is removed from the Engine.
	NULL will unset the primary module.
	Write-locks.
	*/
	void setPrimaryModule(Module* module);
	Module* getPrimaryModule();

	/** Returns the sample rate used by the engine for stepping each module.
	*/
	float getSampleRate();
	/** Sets the sample rate to step the modules.
	Write-locks.
	*/
	INTERNAL void setSampleRate(float sampleRate);
	/** Sets the sample rate if the sample rate in the settings is "Auto".
	Write-locks.
	*/
	void setSuggestedSampleRate(float suggestedSampleRate);
	/** Returns the inverse of the current sample rate.
	*/
	float getSampleTime();
	/** Causes worker threads to block on a mutex instead of spinlock.
	Call this in your Module::stepBlock() method to hint that the operation will take more than ~0.1 ms.
	*/
	void yieldWorkers();
	/** Returns the number of stepBlock() calls since the Engine was created.
	*/
	int64_t getBlock();
	/** Returns the number of audio samples since the Engine was created.
	*/
	int64_t getFrame();
	/** Sets the frame of the next stepBlock() call.
	There is no reason to reset the frame in standalone Rack.
	*/
	void setFrame(int64_t frame);
	/** Returns the frame when stepBlock() was last called.
	*/
	int64_t getBlockFrame();
	/** Returns the time in seconds when stepBlock() was last called.
	*/
	double getBlockTime();
	/** Returns the total number of frames in the current stepBlock() call.
	*/
	int getBlockFrames();
	/** Returns the total time that stepBlock() is advancing, in seconds.
	Calculated by `stepFrames / sampleRate`.
	*/
	double getBlockDuration();

	// Modules
	size_t getNumModules();
	/** Fills `moduleIds` with up to `len` module IDs in the rack.
	Returns the number of IDs written.
	This C-like method does no allocations. The vector C++ version below does.
	Read-locks.
	*/
	size_t getModuleIds(int64_t* moduleIds, size_t len);
	/** Returns a vector of module IDs in the rack.
	Read-locks.
	*/
	std::vector<int64_t> getModuleIds();
	/** Adds a Module to the rack.
	The module ID must not be taken by another Module.
	If the module ID is -1, an ID is automatically assigned.
	Does not transfer pointer ownership.
	Write-locks.
	*/
	void addModule(Module* module);
	/** Removes a Module from the rack.
	Write-locks.
	*/
	void removeModule(Module* module);
	INTERNAL void removeModule_NoLock(Module* module);
	/** Checks whether a Module is in the rack.
	Read-locks.
	*/
	bool hasModule(Module* module);
	/** Returns the Module with the given ID in the rack.
	Read-locks.
	*/
	Module* getModule(int64_t moduleId);
	/** Triggers a ResetEvent for the given Module.
	Write-locks.
	*/
	void resetModule(Module* module);
	/** Triggers a RandomizeEvent for the given Module.
	Write-locks.
	*/
	void randomizeModule(Module* module);
	/** Sets the bypassed state and triggers a BypassEvent or UnBypassEvent of the given Module.
	Write-locks.
	*/
	void bypassModule(Module* module, bool bypassed);
	/** Serializes the given Module with locking, ensuring that Module::process() is not called simultaneously.
	Read-locks.
	*/
	json_t* moduleToJson(Module* module);
	/** Serializes the given Module with locking, ensuring that Module::process() is not called simultaneously.
	Write-locks.
	*/
	void moduleFromJson(Module* module, json_t* rootJ);

	// Cables
	size_t getNumCables();
	/** Fills `cableIds` with up to `len` cable IDs in the rack.
	Returns the number of IDs written.
	This C-like method does no allocations. The vector C++ version below does.
	Read-locks.
	*/
	size_t getCableIds(int64_t* cableIds, size_t len);
	/** Returns a vector of cable IDs in the rack.
	Read-locks.
	*/
	std::vector<int64_t> getCableIds();
	/** Adds a Cable to the rack.
	The cable ID must not be taken by another cable.
	If the cable ID is -1, an ID is automatically assigned.
	Does not transfer pointer ownership.
	Write-locks.
	*/
	void addCable(Cable* cable);
	/** Removes a Cable from the rack.
	Write-locks.
	*/
	void removeCable(Cable* cable);
	INTERNAL void removeCable_NoLock(Cable* cable);
	/** Checks whether a Cable is in the rack.
	Read-locks.
	*/
	bool hasCable(Cable* cable);
	/** Returns the Cable with the given ID in the rack.
	Read-locks.
	*/
	Cable* getCable(int64_t cableId);

	// Params
	void setParam(Module* module, int paramId, float value);
	float getParam(Module* module, int paramId);
	/** Requests the parameter to smoothly change toward `value`.
	*/
	void setSmoothParam(Module* module, int paramId, float value);
	/** Returns the target value before smoothing.
	*/
	float getSmoothParam(Module* module, int paramId);

	// ParamHandles
	/** Adds a ParamHandle to the rack.
	Does not automatically update the ParamHandle.
	Write-locks.
	*/
	void addParamHandle(ParamHandle* paramHandle);
	/**
	Write-locks.
	*/
	void removeParamHandle(ParamHandle* paramHandle);
	INTERNAL void removeParamHandle_NoLock(ParamHandle* paramHandle);
	/** Returns the unique ParamHandle for the given paramId
	Read-locks.
	*/
	ParamHandle* getParamHandle(int64_t moduleId, int paramId);
	/** Use getParamHandle(moduleId, paramId) instead.
	Read-locks.
	*/
	DEPRECATED ParamHandle* getParamHandle(Module* module, int paramId);
	/** Sets the ParamHandle IDs and module pointer.
	If `overwrite` is true and another ParamHandle points to the same param, unsets that one and replaces it with the given handle.
	Read-locks.
	*/
	void updateParamHandle(ParamHandle* paramHandle, int64_t moduleId, int paramId, bool overwrite = true);

	/** Serializes the rack.
	Read-locks.
	*/
	json_t* toJson();
	/** Deserializes the rack.
	Write-locks.
	*/
	void fromJson(json_t* rootJ);
};


} // namespace engine
} // namespace rack
