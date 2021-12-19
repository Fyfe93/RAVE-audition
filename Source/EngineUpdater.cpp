/*
  ==============================================================================

    EngineUpdater.cpp
    Author:  Andrew Fyfe

  ==============================================================================
*/

#include "EngineUpdater.h"

//==============================================================================

UpdateEngineJob::UpdateEngineJob(RAVEAuditionAudioProcessor& processor, const std::string modelFile) : ThreadPoolJob("UpdateEngineJob"), mProcessor(processor), mModelFile(modelFile)
{
}

UpdateEngineJob::~UpdateEngineJob()
{
}

bool UpdateEngineJob::waitForFadeOut(size_t waitTimeMs)
{
    for (size_t i=0; i < waitTimeMs && mProcessor.getIsMuted(); ++i)
    {
        Thread::sleep(1);
    }
    return (mProcessor.getIsMuted());
}

auto UpdateEngineJob::runJob() -> JobStatus
{
    if (shouldExit())
    {
        return JobStatus::jobNeedsRunningAgain;
    }
    
    mProcessor.mute();

    while (!waitForFadeOut(1))
    {
        if (shouldExit())
        {
            return JobStatus::jobNeedsRunningAgain;
        }
    }
    
    mProcessor.mRave->load_model(mModelFile);
    
    mProcessor.unmute();
    
    DBG("Job finished");
    
    return JobStatus::jobHasFinished;
}


//==============================================================================
