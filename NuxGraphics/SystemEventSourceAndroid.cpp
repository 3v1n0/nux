#include "SystemEventSource.h"

#include "Events.h"

#include <hybris/input/input_stack_compatibility_layer.h>
#include <hybris/input/input_stack_compatibility_layer_flags_motion.h>

#include <limits>

namespace nux
{

class SystemEventSourceAndroid : public SystemEventSource
{
  public:
    
    static InputStackConfiguration DefaultInputStackConfiguration()
    {
        InputStackConfiguration config = { true, std::numeric_limits<int>::max() };
        return config;
    }

    static void OnNewInputEvent(::Event* event, void* context)
    {
        if (!context)
            return;

        nux::Event nux_event;
        
        switch(event->type)
        {
        case MOTION_EVENT_TYPE:
            if (event->details.motion.pointer_count == 1) // Translate to mouse event
            {
                nux_event.dx = nux_event.dy = 0;
                nux_event.x = nux_event.x_root = 
                        event->details.motion.pointer_coordinates[0].x;
                nux_event.y = nux_event.y_root = 
                        event->details.motion.pointer_coordinates[0].y;
                if ((event->action & ISCL_MOTION_EVENT_ACTION_MASK) == ISCL_MOTION_EVENT_ACTION_DOWN)
                {
                    nux_event.type = nux::EVENT_MOUSE_DOWN;
                }
                else if ((event->action & ISCL_MOTION_EVENT_ACTION_MASK) == ISCL_MOTION_EVENT_ACTION_MOVE)
                {
                    nux_event.type = nux::EVENT_MOUSE_MOVE;
                }
                else if ((event->action & ISCL_MOTION_EVENT_ACTION_MASK) == ISCL_MOTION_EVENT_ACTION_UP)
                {
                    nux_event.type = nux::EVENT_MOUSE_UP;
                }
                
            }
            break;
        default:
            break;
        }
        
        SystemEventSourceAndroid* thiz = static_cast<SystemEventSourceAndroid*>(context);
        thiz->NotifyListeners(nux_event);

    }

    SystemEventSourceAndroid()
    {
        listener.on_new_event = SystemEventSourceAndroid::OnNewInputEvent;
        listener.context = this;

        static InputStackConfiguration config = DefaultInputStackConfiguration();
        android_input_stack_initialize(
            &listener, 
            &config);

    }

    ~SystemEventSourceAndroid()
    {
        Stop();
        Shutdown();
    }

    void Shutdown()
    {
        android_input_stack_shutdown();
    }

    void PollOnce()
    {
        printf("%s \n", __PRETTY_FUNCTION__);
        android_input_stack_loop_once();
    }

    void Start()
    {
        printf("%s \n", __PRETTY_FUNCTION__);        
        android_input_stack_start();
    }

    void Stop()
    {
        android_input_stack_stop();
    }

  private:
    AndroidEventListener listener;
};

}

namespace
{
nux::SystemEventSourceAndroid* global_instance = NULL;
}

nux::SystemEventSource* nux::SystemEventSource::Instance()
{
    printf("%s \n", __PRETTY_FUNCTION__);
    if (!global_instance)
        global_instance = new nux::SystemEventSourceAndroid();

    return global_instance;
}
