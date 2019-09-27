#ifndef slic3r_Mouse3DController_hpp_
#define slic3r_Mouse3DController_hpp_

#if ENABLE_3DCONNEXION_DEVICES

#include "hidapi/hidapi.h"
#include <thread>
#include <mutex>
#include <vector>

namespace Slic3r {
namespace GUI {

class GLCanvas3D;

class Mouse3DController
{
    class State
    {
        static const double DefaultTranslationScale;
        static const float DefaultRotationScale;

        mutable std::mutex m_mutex;

        Vec3d m_translation;
        Vec3f m_rotation;
        std::vector<bool> m_buttons;

        double m_translation_scale;
        float m_rotation_scale;

    public:
        State();

        void set_translation(const Vec3d& translation);
        void set_rotation(const Vec3f& rotation);
        void set_button(unsigned int id);

        bool has_translation() const;
        bool has_rotation() const;
        bool has_any_button() const;

        // return true if any change to the camera took place
        bool apply(GLCanvas3D& canvas);
    };

    bool m_initialized;
    State m_state;
    std::thread m_thread;
    GLCanvas3D* m_canvas;
    std::mutex m_mutex;
    hid_device* m_device;
    bool m_running;

public:
    Mouse3DController();

    void init();
    void shutdown();

    bool is_device_connected() const { return m_device != nullptr; }
    bool is_running() const { return m_running; }

    void set_canvas(GLCanvas3D* canvas)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_canvas = canvas;
    }

    bool has_translation() const { return m_state.has_translation(); }
    bool has_rotation() const { return m_state.has_rotation(); }
    bool has_any_button() const { return m_state.has_any_button(); }

    bool apply()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return (m_canvas != nullptr) ? m_state.apply(*m_canvas) : false;
    }

private:
    void connect_device();
    void disconnect_device();
    void start();
    void stop() { m_running = false; }

    void run();
    void collect_input();
};

} // namespace GUI
} // namespace Slic3r

#endif // ENABLE_3DCONNEXION_DEVICES

#endif // slic3r_Mouse3DController_hpp_
