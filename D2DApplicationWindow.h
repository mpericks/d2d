#pragma once
#include "GuiHelpers.h"
#include "RenderController.h"
#include "D2DGenericModel.h"

class D2DApplicationWindow : public GuiHelpers::MainWindowBaseImpl
{
public:
    D2DApplicationWindow(GuiHelpers::MessageLoop* message_loop_ptr_init);
    ~D2DApplicationWindow(void);
protected:
    D2DApplicationWindow(void) {};
public:
    virtual LRESULT MessageHandler(HWND window_handle, UINT msg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnCreate(LPCREATESTRUCT create_info_ptr);
    void OnDestroy(HWND window_handle);
    LRESULT OnGetMinMaxSize(HWND window_handle, LPMINMAXINFO min_max_info);
    LRESULT OnSize(HWND window_handle, UINT state, int cx, int cy);
    void OnPaint(HWND window_handle);
    void OnKeyDown(HWND window_handle, UINT virtual_key, BOOL is_down_already, int repeat_count, UINT flags);
    UINT OnGetDlgCode(HWND window_handle, LPMSG msg_ptr);
    void OnMouseLeftButtonDown(HWND window_handle, BOOL is_double_click, int x, int y, UINT_PTR keys_down);
private:
    void RenderIsStale();
private:
    GuiHelpers::MessageLoop* message_loop_ptr;
    D2DGenericModel d2d_model;
    bool in_render_loop;
    RenderController render_controller;
};
