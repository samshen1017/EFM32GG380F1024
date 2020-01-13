#include "littlevgl2rtt.h"
#include "lvgl.h"

#include <rtthread.h>
#include <rtdevice.h>

#include "lv_test_theme.h"

//void btn_click_action(lv_obj_t * obj, lv_event_t event)
lv_res_t btn_click_action(struct _lv_obj_t *obj)
{
    rt_kprintf("HELLO\n");
    return LV_RES_OK;
}

static lv_style_t style_LED;
static lv_style_t style_ZH;
static lv_obj_t *label_Sensor;
static void m_test(lv_theme_t *th, lv_obj_t *parent)
{
    // rt_kprintf("m_test.\n");
    // lv_obj_t *label_SN = lv_label_create(parent, NULL);
    // lv_label_set_text(label_SN, "SN:0060123456");
    // lv_obj_set_pos(label_SN, 120, 50);

    lv_obj_t *label_DatetTime = lv_label_create(parent, NULL);
    lv_label_set_text(label_DatetTime, "07-15 13:10");
    lv_obj_set_pos(label_DatetTime, 15, 3);

    lv_obj_t *label_BT_RSSI = lv_label_create(parent, NULL);
    lv_label_set_text(label_BT_RSSI, "-30db");
    lv_obj_set_pos(label_BT_RSSI, 235, 3);

    lv_obj_t *icon_bat = lv_img_create(parent, NULL);
    lv_img_set_src(icon_bat, "P:/bat.bin");
    lv_obj_set_pos(icon_bat, 335, 2);

    lv_obj_t *icon_bt = lv_img_create(parent, NULL);
    lv_img_set_src(icon_bt, "P:/ble.bin");
    lv_obj_set_pos(icon_bt, 300, 2);

    lv_obj_t *slider_process = lv_slider_create(parent, NULL);
    lv_slider_set_style(slider_process, LV_SLIDER_STYLE_KNOB, &lv_style_transp);
    lv_obj_set_size(slider_process, 380, 13);
    lv_obj_set_pos(slider_process, 10, 40);
    lv_slider_set_value(slider_process, 20);

    lv_style_copy(&style_ZH, &lv_style_scr);
    style_ZH.text.font = FONT_DROIDSANSFALLBACK_20;
    lv_obj_t *label_max = lv_label_create(parent, NULL);
    lv_obj_set_style(label_max, &style_ZH);
    lv_label_set_text(label_max, "最大: 100.99");
    lv_obj_set_pos(label_max, 15, 75);

    lv_obj_t *label_min = lv_label_create(parent, NULL);
    lv_obj_set_style(label_min, &style_ZH);
    lv_label_set_text(label_min, "最小: 0.1234");
    lv_obj_set_pos(label_min, 15, 105);

    lv_obj_t *label_unit = lv_label_create(parent, NULL);
    lv_obj_set_style(label_unit, &style_ZH);
    lv_label_set_text(label_unit, "单位: MPa");
    lv_obj_set_pos(label_unit, 15, 135);

    // lv_obj_t *label;
    // lv_obj_t *btn1 = lv_btn_create(parent, NULL);
    // label = lv_label_create(btn1, NULL);
    // lv_label_set_text(label, "MENU");
    // lv_obj_set_size(btn1, 60, 40);
    // lv_obj_set_pos(btn1, 20, 190);
    // lv_btn_set_action(btn1, LV_BTN_ACTION_CLICK, btn_click_action);

    // lv_obj_t *btn2 = lv_btn_create(parent, NULL);
    // label = lv_label_create(btn2, NULL);
    // lv_label_set_text(label, "INFO");
    // lv_obj_set_size(btn2, 60, 40);
    // lv_obj_set_pos(btn2, 160, 190);
    // lv_btn_set_action(btn2, LV_BTN_ACTION_CLICK, btn_click_action);

    // lv_obj_t *btn3 = lv_btn_create(parent, NULL);
    // label = lv_label_create(btn3, NULL);
    // lv_label_set_text(label, "SET");
    // lv_obj_set_size(btn3, 60, 40);
    // lv_obj_set_pos(btn3, 310, 190);
    // lv_btn_set_action(btn3, LV_BTN_ACTION_CLICK, btn_click_action);

    lv_style_copy(&style_LED, &lv_style_scr);
    style_LED.text.font = FONT_UNIDREAM_LED_110;
    //lv_obj_t *label_Sensor = lv_label_create(parent, NULL);
    label_Sensor = lv_label_create(parent, NULL);
    lv_obj_set_style(label_Sensor, &style_LED);
    lv_label_set_text(label_Sensor, "-190.00");
    lv_obj_set_pos(label_Sensor, 150, 58);

    lv_obj_t *btn_Home = lv_imgbtn_create(parent, NULL);
    lv_imgbtn_set_src(btn_Home, LV_BTN_STATE_REL, "P:/src/home.bin");
    lv_imgbtn_set_src(btn_Home, LV_BTN_STATE_PR, "P:/src/home_p.bin");
    lv_imgbtn_set_src(btn_Home, LV_BTN_STATE_TGL_REL, "P:/src/home.bin");
    lv_imgbtn_set_src(btn_Home, LV_BTN_STATE_TGL_PR, "P:/src/home_p.bin");
    //lv_imgbtn_set_toggle(btn_Home, true);
    lv_obj_set_pos(btn_Home, 20, 186);
    lv_btn_set_action(btn_Home, LV_BTN_ACTION_CLICK, btn_click_action);

    lv_obj_t *btn_Chart = lv_imgbtn_create(parent, NULL);
    lv_imgbtn_set_src(btn_Chart, LV_BTN_STATE_REL, "P:/src/chart.bin");
    lv_imgbtn_set_src(btn_Chart, LV_BTN_STATE_PR, "P:/src/chart_p.bin");
    lv_imgbtn_set_src(btn_Chart, LV_BTN_STATE_TGL_REL, "P:/src/chart.bin");
    lv_imgbtn_set_src(btn_Chart, LV_BTN_STATE_TGL_PR, "P:/src/chart_p.bin");
    //lv_imgbtn_set_toggle(btn_Chart, false);
    lv_obj_set_pos(btn_Chart, 160, 185);
    lv_btn_set_action(btn_Chart, LV_BTN_ACTION_CLICK, btn_click_action);

    lv_obj_t *btn_Settings = lv_imgbtn_create(parent, NULL);
    lv_imgbtn_set_src(btn_Settings, LV_BTN_STATE_REL, "P:/src/settings.bin");
    lv_imgbtn_set_src(btn_Settings, LV_BTN_STATE_PR, "P:/src/settings_p.bin");
    lv_imgbtn_set_src(btn_Settings, LV_BTN_STATE_TGL_REL, "P:/src/settings.bin");
    lv_imgbtn_set_src(btn_Settings, LV_BTN_STATE_TGL_PR, "P:/src/settings_p.bin");
    //lv_imgbtn_set_toggle(btn_Settings, true);
    lv_obj_set_pos(btn_Settings, 310, 185);
    lv_btn_set_action(btn_Settings, LV_BTN_ACTION_CLICK, btn_click_action);
}

static void lvgl_demo_run(void *p)
{
    //lv_theme_t *th = lv_theme_material_init(210, LV_FONT_DEFAULT);
    lv_theme_t *th = lv_theme_mono_init(0, LV_FONT_DEFAULT);
    RT_ASSERT(th != NULL);
    //lv_test_theme(th);
    lv_theme_set_current(th);
    lv_obj_t *scr = lv_cont_create(NULL, NULL);
    lv_scr_load(scr);
    m_test(th, scr);
    double i = 99.12;
    char *str[25] = {0};
    while (1)
    {
        rt_thread_delay(RT_TICK_PER_SECOND / 100);
        // rt_thread_delay(500);
        // rt_memset(str, 0, 25);
        // sprintf(str, "%.2f", i);
        // lv_label_set_text(label_Sensor, str);
        // i += 0.3;
        lv_task_handler();
    }
}

int lvgl_test(void)
{
    rt_err_t ret = RT_EOK;
    rt_thread_t thread = RT_NULL;
    /* find lcd device */
    rt_device_t lcd = rt_device_find("memlcd");
    if (lcd == RT_NULL)
    {
        rt_kprintf("Can't find LCD\n");
        return RT_ERROR;
    }
    rtgui_graphic_set_device(lcd);
    /* init rtgui system server */
    rtgui_system_server_init();
    /* init littlevGL */
    ret = littlevgl2rtt_init("memlcd");
    if (ret != RT_EOK)
    {
        return ret;
    }
    /* littleGL demo gui thread */
    thread = rt_thread_create("lv_demo", lvgl_demo_run, RT_NULL, 4 * 1024, 5, 10);
    if (thread == RT_NULL)
    {
        return RT_ERROR;
    }
    rt_thread_startup(thread);

    return RT_EOK;
}
//INIT_APP_EXPORT(rt_lvgl_demo_init);
FINSH_FUNCTION_EXPORT(lvgl_test, lvgl_test);
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(lvgl_test, lvgl_test);
#endif
