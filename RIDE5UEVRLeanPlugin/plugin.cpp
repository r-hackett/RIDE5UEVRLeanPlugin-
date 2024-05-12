#define NOMINMAX

#include "uevr/Plugin.hpp"

using namespace uevr;

class Ride5LeanPlugin : public uevr::Plugin
{
public:
    Ride5LeanPlugin() : m_lean(.0f), m_max_difference(.3f) {}

    void on_dllmain() override {}

    void on_initialize() override
    {
        API::get()->log_info("Lean plugin initialized.");
    }

    void on_pre_engine_tick(API::UGameEngine* engine, float delta) override
    {
        const UEVR_VRData* vr = API::get()->param()->vr;

        if (!vr->is_runtime_ready())
            return;

        UEVR_Vector3f left_position, right_position;
        UEVR_Quaternionf left_rotation, right_rotation;

        vr->get_pose(vr->get_left_controller_index(), &left_position, &left_rotation);
        vr->get_pose(vr->get_right_controller_index(), &right_position, &right_rotation);

        float difference = left_position.y - right_position.y;

        // Normalize and clamp the height difference to [-1.0, 1.0]
        m_lean = std::max(-1.0f, std::min(1.0f, difference / m_max_difference));
    }

    void on_xinput_get_state(uint32_t* retval, uint32_t user_index, XINPUT_STATE* state) override
    {
        state->Gamepad.sThumbLX = static_cast<SHORT>(m_lean * 32767.f);
    }

private:
    float m_lean;
    float m_max_difference;
};

std::unique_ptr<Ride5LeanPlugin> g_plugin{ new Ride5LeanPlugin() };
