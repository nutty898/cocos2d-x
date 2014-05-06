/****************************************************************************
Copyright (c) 2013 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __CCTIMELINE_ACTION_H__
#define __CCTIMELINE_ACTION_H__

#include "cocos2d.h"
#include "CCTimeLine.h"
#include "renderer/CCRenderer.h"

namespace cocostudio {
namespace timeline{

typedef struct _FrameEvent
{
    cocos2d::Node* node;
    int eventTag;
    int originFrameIndex;
    int currentFrameIndex;
}FrameEvent;

class TimelineAction : public cocos2d::Action
{
public:
    static TimelineAction* create();

    TimelineAction();
    virtual ~TimelineAction();

    virtual bool init();

    /** Goto the specified frame index, and start playing from this index.
     * @param startIndex The animation will play from this index.
     */
    virtual void gotoFrameAndPlay(int startIndex);

    /** Goto the specified frame index, and start playing from this index.
     * @param startIndex The animation will play from this index.
     * @param loop Whether or not the animation need loop. 
     */
    virtual void gotoFrameAndPlay(int startIndex, bool loop);

    /** Goto the specified frame index, and start playing from start index, end at end index.
     * @param startIndex The animation will play from this index.
     * @param endIndex The animation will end at this index.
     * @param loop Whether or not the animation need loop. 
     */
    virtual void gotoFrameAndPlay(int startIndex, int endIndex, bool loop);

    /** Goto the specified frame index, and pause at this index.
     * @param startIndex The animation will pause at this index.
     */
    virtual void gotoFrameAndPause(int startIndex);

    /** Pause the animation. */
    virtual void pause();
    /** Resume the animation. */
    virtual void resume();

    /** Set the animation speed, this will speed up or slow down the speed. */
    virtual void  setTimeSpeed(float speed);
    /** Get current animation speed. */
    virtual float getTimeSpeed();

    /** */
    void setFrameEventCallFunc(std::function<void(FrameEvent*)> listener);

    /** duration of the whole action*/
    virtual void setDuration(int duration) { _duration = duration; }
    virtual int  getDuration() { return _duration; }

    /** End frame of this action.
      * When action play to this frame, if action is not loop, then it will stop, 
      * or it will play from start frame again. */
    virtual void setEndFrame(int endFrame) { _endFrame = endFrame; }
    virtual int  getEndFrame() { return _endFrame; }

    /** Get current frame. */
    virtual int  getCurrentFrame() { return _currentFrame; }

    void addTimeline(Timeline* timeline);
    void removeTimeline(Timeline* timeline);


    /** Inherit from cocos2d::Action. */

    /** Returns a clone of TimelineAction */
    virtual TimelineAction* clone() const override; 

    /** Returns a reverse of TimelineAction. 
     *  Not implement yet.
     */
    virtual TimelineAction* reverse() const override { return nullptr; }

    virtual void step(float delta) override; 
    virtual void startWithTarget(cocos2d::Node *target) override;  
    virtual bool isDone() const override { return false; }
protected:
    virtual void gotoFrame(int frameIndex);

    std::map<int, cocos2d::Vector<Timeline*>> _timelineMap;
    cocos2d::Vector<Timeline*> _timelineList;

    int     _duration;
    double  _time;
    float   _timeSpeed;
    float   _frameInternal;
    bool    _playing;
    int     _currentFrame;
    int     _endFrame;
    bool    _loop;
};

}
}


#endif /*__CCTIMELINE_ACTION_H__*/
