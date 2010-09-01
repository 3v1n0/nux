/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranties of 
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the applicable version of the GNU Lesser General Public 
 * License for more details.
 * 
 * You should have received a copy of both the GNU Lesser General Public 
 * License version 3 along with this program.  If not, see 
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef CAMERA_H
#define CAMERA_H

#if defined(INL_OS_WINDOWS)
    #include <xinput.h>
#endif
#include "VLayout.h"

NAMESPACE_BEGIN_GUI

class GfxSimulation;

class MouseController
{
public:
    MouseController()
        : oldx_(0), oldy_(0), newx_(0), newy_(0), right_mouse_up_(true), left_mouse_up_(true),
        phi_(0), theta_(0), dx_(0), dy_(0)
    {};
    ~MouseController(){};

    //Update();

    void set_xy(int x, int y)
    {
        newx_ = x;
        newy_ = y;
    }

    void set_xy_old(int x, int y)
    {
        oldx_ = x;
        oldy_ = y;
    }

    void set_phi(float p)   {phi_ = p;}
    float get_phi() const {return phi_;}
    void set_theta(float t) {theta_ = t;}
    float get_theta() const {return theta_;}

    void set_dx(int x) {dx_ = x;}
    void set_dy(int y) {dy_ = y;}
    int get_dx() const {return dx_;}
    int get_dy() const {return dy_;}

    int difference_x() {return (newx_ - oldx_);}
    int difference_y() {return (newy_ - oldy_);}

    void right_mouse_up(bool b) {right_mouse_up_ = b;}    
    bool is_right_mouse_up() {return right_mouse_up_;}

    void left_mouse_up(bool b) {left_mouse_up_ = b;}    
    bool is_left_mouse_up() {return left_mouse_up_;}

    int dx_, dy_;

private:
    bool left_mouse_up_;
    bool right_mouse_up_;
    int oldx_, oldy_;
    int newx_, newy_;
    float phi_, theta_;
};

// enum D3DUtil_CameraKeys
// {
//     CAM_STRAFE_LEFT = 0,
//     CAM_STRAFE_RIGHT,
//     CAM_MOVE_FORWARD,
//     CAM_MOVE_BACKWARD,
//     CAM_MOVE_UP,
//     CAM_MOVE_DOWN,
//     CAM_RESET,
//     CAM_CONTROLDOWN,
//     CAM_MAX_KEYS,
//     CAM_UNKNOWN     = 0xFF
// };
// 
// #define KEY_WAS_DOWN_MASK 0x80
// #define KEY_IS_DOWN_MASK  0x01
// 
// #define MOUSE_LEFT_BUTTON   0x01
// #define MOUSE_MIDDLE_BUTTON 0x02
// #define MOUSE_RIGHT_BUTTON  0x04
// #define MOUSE_WHEEL         0x08
// 
// //--------------------------------------------------------------------------------------
// // XInput helper state/function
// // This performs extra processing on XInput gamepad data to make it slightly more convenient to use
// // 
// // Example usage:
// //
// //      DXUT_GAMEPAD gamepad[4];
// //      for( DWORD iPort=0; iPort<DXUT_MAX_CONTROLLERS; iPort++ )
// //          DXUTGetGamepadState( iPort, gamepad[iPort] );
// //
// //--------------------------------------------------------------------------------------
// #define DXUT_MAX_CONTROLLERS 4  // XInput handles up to 4 controllers 
// 
// struct DXUT_GAMEPAD
// {
//     // From XINPUT_GAMEPAD
//     WORD wButtons;
//     BYTE bLeftTrigger;
//     BYTE bRightTrigger;
//     SHORT sThumbLX;
//     SHORT sThumbLY;
//     SHORT sThumbRX;
//     SHORT sThumbRY;
// 
//     // Device properties
//     XINPUT_CAPABILITIES caps;
//     bool bConnected; // If the controller is currently connected
//     bool bInserted;  // If the controller was inserted this frame
//     bool bRemoved;   // If the controller was removed this frame
// 
//     // Thumb stick values converted to range [-1,+1]
//     float fThumbRX;
//     float fThumbRY;
//     float fThumbLX;
//     float fThumbLY;
// 
//     // Records which buttons were pressed this frame.
//     // These are only set on the first frame that the button is pressed
//     WORD wPressedButtons;
//     bool bPressedLeftTrigger;
//     bool bPressedRightTrigger;
// 
//     // Last state of the buttons
//     WORD wLastButtons;
//     bool bLastLeftTrigger;
//     bool bLastRightTrigger;
// };
// 
// //--------------------------------------------------------------------------------------
// // Simple base camera class that moves and rotates.  The base class
// //       records mouse and keyboard input for use by a derived class, and 
// //       keeps common state.
// //--------------------------------------------------------------------------------------
// class CBaseCamera
// {
// public:
//     CBaseCamera();
// 
//     // Call these from client and use Get*Matrix() to read new matrices
//     virtual LRESULT             HandleMessages(IEvent& ievent, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
//     virtual void                FrameMove( float fElapsedTime ) = 0;
// 
//     // Functions to change camera matrices
//     virtual void                Reset();
//     virtual void                SetViewParams(const Vector3& pvEyePt, const Vector3& pvLookatPt );
//     virtual void                SetProjParams( float fFOV, float fAspect, float fNearPlane, float fFarPlane );
// 
//     // Functions to change behavior
//     virtual void SetDragRect( Rect &rc ) { m_rcDrag = rc; }
//     void SetInvertPitch( bool bInvertPitch ) { m_bInvertPitch = bInvertPitch; }
//     void SetDrag( bool bMovementDrag, float fTotalDragTimeToZero = 0.25f ) { m_bMovementDrag = bMovementDrag; m_fTotalDragTimeToZero = fTotalDragTimeToZero; }
//     void SetEnableYAxisMovement( bool bEnableYAxisMovement ) { m_bEnableYAxisMovement = bEnableYAxisMovement; }
//     void SetEnablePositionMovement( bool bEnablePositionMovement ) { m_bEnablePositionMovement = bEnablePositionMovement; }
//     void SetClipToBoundary( bool bClipToBoundary, Vector3* pvMinBoundary, Vector3* pvMaxBoundary ) { m_bClipToBoundary = bClipToBoundary; if( pvMinBoundary ) m_vMinBoundary = *pvMinBoundary; if( pvMaxBoundary ) m_vMaxBoundary = *pvMaxBoundary; }
//     void SetScalers( float fRotationScaler = 0.01f, float fMoveScaler = 5.0f )  { m_fRotationScaler = fRotationScaler; m_fMoveScaler = fMoveScaler; }
//     void SetNumberOfFramesToSmoothMouseData( int nFrames ) { if( nFrames > 0 ) m_fFramesToSmoothMouseData = (float)nFrames; }
//     void SetResetCursorAfterMove( bool bResetCursorAfterMove ) { m_bResetCursorAfterMove = bResetCursorAfterMove; }
// 
//     // Functions to get state
//     const Matrix4*  GetViewMatrix() const { return &m_mView; }
//     const Matrix4*  GetProjMatrix() const { return &m_mProj; }
//     const Vector3* GetEyePt() const      { return &m_vEye; }
//     const Vector3* GetLookAtPt() const   { return &m_vLookAt; }
//     float GetNearClip() const { return m_fNearPlane; }
//     float GetFarClip() const { return m_fFarPlane; }
// 
//     bool IsBeingDragged() const         { return (m_bMouseLButtonDown || m_bMouseMButtonDown || m_bMouseRButtonDown); }
//     bool IsMouseLButtonDown() const     { return m_bMouseLButtonDown; } 
//     bool IsMouseMButtonDown() const     { return m_bMouseMButtonDown; } 
//     bool IsMouseRButtonDown() const     { return m_bMouseRButtonDown; } 
// 
// protected:
//     // Functions to map a WM_KEYDOWN key to a D3DUtil_CameraKeys enum
//     virtual D3DUtil_CameraKeys  MapKey( t_u32 nKey );
//     bool IsKeyDown( t_u8 key ) const { return( (key & KEY_IS_DOWN_MASK) == KEY_IS_DOWN_MASK ); }
//     bool WasKeyDown( t_u8 key ) const { return( (key & KEY_WAS_DOWN_MASK) == KEY_WAS_DOWN_MASK ); }
// 
//     void                        ConstrainToBoundary( Vector3* pV );
//     void                        UpdateMouseDelta();
//     void                        UpdateVelocity( float fElapsedTime );
//     void GetInput( bool bGetKeyboardInput, bool bGetMouseInput, bool bGetGamepadInput, bool bResetCursorAfterMove );
// 
//     Matrix4 m_mView;              // View matrix 
//     Matrix4 m_mProj;              // Projection matrix
// 
//     DXUT_GAMEPAD                m_GamePad[DXUT_MAX_CONTROLLERS];  // XInput controller state
//     Vector3 m_vGamePadLeftThumb;
//     Vector3 m_vGamePadRightThumb;
//     //double                      m_GamePadLastActive[DXUT_MAX_CONTROLLERS];
// 
//     int m_cKeysDown;            // Number of camera keys that are down.
//     t_u8                        m_aKeys[CAM_MAX_KEYS];  // State of input - KEY_WAS_DOWN_MASK|KEY_IS_DOWN_MASK
//     Vector3 m_vKeyboardDirection;   // Direction vector of keyboard input
//     Point m_ptLastMousePosition;  // Last absolute position of mouse cursor
//     bool m_bMouseLButtonDown;    // True if left button is down 
//     bool m_bMouseMButtonDown;    // True if middle button is down 
//     bool m_bMouseRButtonDown;    // True if right button is down 
//     int m_nCurrentButtonMask;   // mask of which buttons are down
//     int m_nMouseWheelDelta;     // Amount of middle wheel scroll (+/-) 
//     Vector2 m_vMouseDelta;          // Mouse relative delta smoothed over a few frames
//     float m_fFramesToSmoothMouseData; // Number of frames to smooth mouse data over
// 
//     Vector3 m_vDefaultEye;          // Default camera eye position
//     Vector3 m_vDefaultLookAt;       // Default LookAt position
//     Vector3 m_vEye;                 // Camera eye position
//     Vector3 m_vLookAt;              // LookAt position
//     float m_fCameraYawAngle;      // Yaw angle of camera
//     float m_fCameraPitchAngle;    // Pitch angle of camera
// 
//     Rect m_rcDrag;               // Rectangle within which a drag can be initiated.
//     Vector3 m_vVelocity;            // Velocity of camera
//     bool m_bMovementDrag;        // If true, then camera movement will slow to a stop otherwise movement is instant
//     Vector3 m_vVelocityDrag;        // Velocity drag force
//     float m_fDragTimer;           // Countdown timer to apply drag
//     float m_fTotalDragTimeToZero; // Time it takes for velocity to go from full to 0
//     Vector2 m_vRotVelocity;         // Velocity of camera
// 
//     float m_fFOV;                 // Field of view
//     float m_fAspect;              // Aspect ratio
//     float m_fNearPlane;           // Near plane
//     float m_fFarPlane;            // Far plane
// 
//     float m_fRotationScaler;      // Scaler for rotation
//     float m_fMoveScaler;          // Scaler for movement
// 
//     bool m_bInvertPitch;         // Invert the pitch axis
//     bool m_bEnablePositionMovement; // If true, then the user can translate the camera/model 
//     bool m_bEnableYAxisMovement; // If true, then camera can move in the y-axis
// 
//     bool m_bClipToBoundary;      // If true, then the camera will be clipped to the boundary
//     Vector3 m_vMinBoundary;         // Min point in clip boundary
//     Vector3 m_vMaxBoundary;         // Max point in clip boundary
// 
//     bool m_bResetCursorAfterMove;// If true, the class will reset the cursor position so that the cursor always has space to move 
// };
// 
// 
// //--------------------------------------------------------------------------------------
// // Simple first person camera class that moves and rotates.
// //       It allows yaw and pitch but not roll.  It uses WM_KEYDOWN and 
// //       GetCursorPos() to respond to keyboard and mouse input and updates the 
// //       view matrix based on input.  
// //--------------------------------------------------------------------------------------
// class CFirstPersonCamera : public CBaseCamera
// {
// public:
//     CFirstPersonCamera();
// 
//     // Call these from client and use Get*Matrix() to read new matrices
//     virtual void    FrameMove( float fElapsedTime );
// 
//     // Functions to change behavior
//     void            SetRotateButtons( bool bLeft, bool bMiddle, bool bRight, bool bRotateWithoutButtonDown = false );
// 
//     // Functions to get state
//     Matrix4*  GetWorldMatrix()            { return &m_mCameraWorld; }
// 
//     const Vector3 GetWorldRight()  const { return Vector3(m_mCameraWorld.m[0][0], m_mCameraWorld.m[1][0], m_mCameraWorld.m[2][0]); } 
//     const Vector3 GetWorldUp()     const { return Vector3(m_mCameraWorld.m[0][1], m_mCameraWorld.m[1][1], m_mCameraWorld.m[2][1]); }
//     const Vector3 GetWorldAhead()  const { return Vector3(m_mCameraWorld.m[0][2], m_mCameraWorld.m[1][2], m_mCameraWorld.m[2][2]); }
//     const Vector3 GetEyePt()       const { return Vector3(m_mCameraWorld.m[0][3], m_mCameraWorld.m[1][3], m_mCameraWorld.m[2][3]); }
// 
// protected:
//     Matrix4 m_mCameraWorld;       // World matrix of the camera (inverse of the view matrix)
// 
//     int m_nActiveButtonMask;  // Mask to determine which button to enable for rotation
//     bool m_bRotateWithoutButtonDown;
// };

NAMESPACE_END_GUI

#endif // CAMERA_H
