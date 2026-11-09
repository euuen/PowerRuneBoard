cmake_minimum_required(VERSION 3.22)
enable_language(C ASM)

add_library(applications INTERFACE)

target_sources(applications INTERFACE
    key/rings_key.c

    light/board_light.c
    light/frame_down_light.c
    light/frame_light.c
    light/frame_up_light.c
    light/running_light.c
    light/ws2812.c

    protocol/protocol.c

    usart/usart_ex.c

    board.c
    fsm.c

    # test
    # app_test/usart.c
    # app_test/can.c
    # app_test/key.c
    # app_test/frame_light.c
    # app_test/frame_light_uncontrol.c
    # app_test/running_light.c
    # app_test/running_light_uncontrol.c
    # app_test/frame_up_light.c
    # app_test/frame_up_light_unctronl.c
    # app_test/frame_down_light.c
    # app_test/frame_down_light_uncontrol.c
    # app_test/board_light.c
    # app_test/board_light_send_all_time.c
    # app_test/board_light_uncontrol.c
    # app_test/all_light.c
    # app_test/running_arrow.c
    # app_test/running_arrow_uncontrol.c
    # app_test/board_ring.c
    # app_test/board_cross.c
    # app_test/ready_state_light.c
)

target_include_directories(applications INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)


