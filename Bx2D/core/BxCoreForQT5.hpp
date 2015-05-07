#pragma once
#include <QApplication>
#include <QEventLoop>
#include <QTcpSocket>
#include <QTcpServer>
#include <QHostInfo>
#include <QThread>
#include <QMutex>
#include <QSemaphore>
#include <QTime>
#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#ifdef __BX_DEBUG
    #include <QMessageBox>
#endif

#include <QMainWindow>
#include <QGLWidget>
#include <QGLShaderProgram>
#include <QMouseEvent>
#include <QFontDatabase>
#include <QStandardPaths>
#include <QLibrary>
#if defined(WIN32)
    #include <QBluetoothDeviceDiscoveryAgent>
#endif
#include <QDesktopWidget>
#include <QWindow>
#include <5.4.0/QtGui/qpa/qplatformnativeinterface.h>
#include <QAbstractNativeEventFilter>

#ifdef OpenMutex
    #undef OpenMutex
#endif

#include <BxSingleton.hpp>
#include <BxPool.hpp>
#include <BxCore.hpp>
#include <BxScene.hpp>
#include <BxVarMap.hpp>
#include <BxVarQueue.hpp>
#include <BxExpress.hpp>
#include <BxMemory.hpp>

#if TARGET_OS_IPHONE == 1
	#define GLSCALE 2
#else
	#define GLSCALE 1
#endif

namespace BxCore
{
	namespace Simulator
    {
		/// @cond SECTION_NAME
		class EventFilter : public QAbstractNativeEventFilter
		{
		public:
			BxVarMap<callback_windowevent> EventCB;
			BxVarMap<void*> EventData;

		public:
			bool nativeEventFilter(const QByteArray& eventType, void* message, long* result)
			{
                #ifdef WIN32
                    if(eventType == "windows_generic_MSG")
                    {
                        MSG* msg = (MSG*) message;
                        if(msg->hwnd == (HWND) BxCore::Simulator::GetWindowHandle())
                        {
                            callback_windowevent* cb = EventCB.Access(msg->message);
                            if(cb)
                            {
                                (*cb)(msg->message, msg->wParam, msg->lParam, EventData[msg->message]);
                                return true;
                            }
                        }
                    }
                #elif defined(Q_OS_MACX)
                    if(eventType == "windows_generic_MSG")
                    {
                    }
                #endif
				return false;
			}
		};
		/// @endcond
	}

	namespace Main
    {
		/// @cond SECTION_NAME
        class GLWidget : public QGLWidget
        {
            Q_OBJECT
        public:
            static GLWidget& Me() {static GLWidget* _ = new GLWidget(); return *_;}

        private:
			enum {DClickRadius = 100, DClickRadiusMin = 20, DClickRadiusSpeed = 10};
			GLuint LastTexture;
			bool InputPressed[inputkind_max];
			int SavedTouchAngle;
            uint SavedTouchFlag;
            float SavedTouchPosX[32];
            float SavedTouchPosY[32];
			QTimer WheelMoveTimer;
			QTimer WheelEndTimer;
			int RadiusForWheel;
            bool TabletPressed;
            bool FirstMoved;
            bool FirstSized;
            int PosX;
            int PosY;
            int SizeW;
            int SizeH;
			BxCore::Simulator::EventFilter Filter;

        private:
            GLWidget() : QGLWidget(QGLFormat(QGL::SampleBuffers)),
                SavedTouchAngle(0), SavedTouchFlag(0), WheelMoveTimer(this), WheelEndTimer(this), RadiusForWheel(0),
                TabletPressed(false), FirstMoved(false), FirstSized(false), PosX(0), PosY(0), SizeW(0), SizeH(0)
            {
				LastTexture = 0;
				for(int i = 0; i < inputkind_max; ++i)
					InputPressed[i] = false;

                setWindowTitle(BxCore::System::GetConfigString("Bx.Currently.Title", ""));
                #ifdef Q_OS_MACX
                    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
                #else
                    setWindowFlags(Qt::FramelessWindowHint);
                #endif
                setAttribute(Qt::WA_PaintOnScreen);
                setAttribute(Qt::WA_NoSystemBackground);
                setAttribute(Qt::WA_AcceptTouchEvents);
                setAutoBufferSwap(false);
                setAutoFillBackground(false);
				connect(&WheelMoveTimer, SIGNAL(timeout()), this, SLOT(WheelMove()));
				connect(&WheelEndTimer, SIGNAL(timeout()), this, SLOT(WheelEnd()));
            }
            virtual ~GLWidget() {}

        public:
			bool IsTouchPressed()
            {
                return InputPressed[inputkind_finger] | InputPressed[inputkind_wacom_pen] | InputPressed[inputkind_mouse_left];
            }
			bool IsSpecialPressed()
            {
                return InputPressed[inputkind_mouse_right] | InputPressed[inputkind_mouse_wheel];
            }
            void BindTexture(GLuint texture)
            {
				if(LastTexture != texture)
				{
					LastTexture = texture;
					glBindTexture(GL_TEXTURE_2D, texture);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_NEAREST
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}
            }
            void SetWindowRect(int x, int y, int w, int h)
            {
                FirstMoved = true;
                FirstSized = true;
                PosX = x;
                PosY = y;
                SizeW = w;
                SizeH = h;
				resize(w, h);
				move(x, y);
            }
			BxCore::Simulator::EventFilter& GetFilter()
			{return Filter;}

			void SetVirtualTouch(inputkind kind, float x, float y, float force, bool pressed)
            {
				if(InputPressed[kind] != pressed)
					PostMouseEvent((pressed)? systouchtype_down : systouchtype_up, 0, x, y, force, kind);
				else if(pressed) PostMouseEvent(systouchtype_move, 0, x, y, force, kind);
                InputPressed[kind] = pressed;
            }

        public slots:
            void RunApplication()
            {
                global_data bool IsFirstTime = true;
                if(IsFirstTime)
                {
                    IsFirstTime = false;
                    makeCurrent();
                    BxCore::Main::RunApplication();
                    close();
                }
            }
			void WheelMove()
			{
				if(InputPressed[inputkind_mouse_wheel])
				{
					point CursorPos = BxCore::Simulator::GetCursorPos();
					point WindowPos = BxCore::Simulator::GetWindowPos();
					const int X = CursorPos.x - WindowPos.x;
					const int Y = CursorPos.y - WindowPos.y;
					PostMouseEvent(systouchtype_move, 0, X, Y, 1, inputkind_mouse_wheel, -RadiusForWheel);
					PostMouseEvent(systouchtype_move, 1, X, Y, 1, inputkind_mouse_wheel, +RadiusForWheel);
				}
			}
			void WheelEnd()
			{
				WheelMoveTimer.stop();
				WheelEndTimer.stop();
				PostMouseEventsByWheelDone();
			}

        protected:
            void initializeGL()
            {
                glClearColor(0.0, 0.0, 0.0, 1.0);
                glClear(GL_COLOR_BUFFER_BIT);
            }
            void resizeGL(int width, int height)
            {
                glViewport(0, 0, width, height);
            }
            void resizeEvent(QResizeEvent* event)
            {
				QGLWidget::resizeEvent(event);
                if(FirstSized)
                {
                    FirstSized = false;
                    resize(SizeW, SizeH);
                }
				else
				{
					const int GUIMarginL = BxCore::Main::GetGUIMarginL();
					const int GUIMarginT = BxCore::Main::GetGUIMarginT();
					const int GUIMarginR = BxCore::Main::GetGUIMarginR();
					const int GUIMarginB = BxCore::Main::GetGUIMarginB();
					SizeW = event->size().width() - GUIMarginL - GUIMarginR;
					SizeH = event->size().height() - GUIMarginT - GUIMarginB;

					sysevent Event;
					Event.type = syseventtype_resize;
					Event.resize.w = SizeW;
					Event.resize.h = SizeH;
					BxScene::__AddEvent__(Event, syseventset_null);
				}
            }
            void moveEvent(QMoveEvent* event)
            {
				QGLWidget::moveEvent(event);
                if(FirstMoved)
                {
                    FirstMoved = false;
                    move(PosX, PosY);
                }
            }
            void paintEvent(QPaintEvent*)
            {
                global_data QTimer* Timer = nullptr;
                delete Timer;
                Timer = new QTimer(this);
                connect(Timer, SIGNAL(timeout()), this, SLOT(RunApplication()));
                Timer->start(1); // <!> Timer대신 몇번째 paintEvent가 게임루프로 적당한지 조사필요
            }
            void closeEvent(QCloseEvent*)
            {
                BxCore::System::DoQuit();
            }
            bool event(QEvent* event)
            {
				switch(event->type())
                {
				case QEvent::WindowActivate:
					QCoreApplication::instance()->installNativeEventFilter(&Filter);
					break;
				case QEvent::WindowDeactivate:
					QCoreApplication::instance()->removeNativeEventFilter(&Filter);
					break;
                case QEvent::TouchBegin:
                case QEvent::TouchUpdate:
                    touchEvent(static_cast<QTouchEvent*>(event));
                    return true;
                case QEvent::TouchEnd:
                    touchEventDone();
                    return true;
                }
                return QWidget::event(event);
            }
            void touchEvent(QTouchEvent* event)
            {
                if(SavedTouchFlag == 0)
                {
                    PostMouseEventsByWheelDone();
                    InputPressed[inputkind_finger] = true;
                }
                uint CurTouchFlag = 0;
                QList<QTouchEvent::TouchPoint> Points = event->touchPoints();
                foreach(const QTouchEvent::TouchPoint& Point, Points)
                {
                    if(Point.state() == Qt::TouchPointStationary)
                        continue;
                    const int ID = Point.id();
                    if(31 < ID) continue;
                    const float X = (float) Point.pos().x();
                    const float Y = (float) Point.pos().y();
                    const bool IsPressed = ((SavedTouchFlag & (1 << ID)) != 0);
                    PostMouseEvent((IsPressed)? systouchtype_move : systouchtype_down, ID, X, Y, 1, inputkind_finger);
                    CurTouchFlag |= 1 << ID;
                    SavedTouchPosX[ID] = X;
                    SavedTouchPosY[ID] = Y;
                }
                for(int i = 0; i < 32; ++i)
                {
                    const bool IsPressed = ((SavedTouchFlag & (1 << i)) != 0);
                    const bool IsPressNow = ((CurTouchFlag & (1 << i)) != 0);
                    if(IsPressed && !IsPressNow)
                        PostMouseEvent(systouchtype_up, i, SavedTouchPosX[i], SavedTouchPosY[i], 1, inputkind_finger);
                }
                SavedTouchFlag = CurTouchFlag;
            }
            void touchEventDone()
            {
                InputPressed[inputkind_finger] = false;
                for(int i = 0; i < 32; ++i)
                    if((SavedTouchFlag & (1 << i)) != 0)
                        PostMouseEvent(systouchtype_up, i, SavedTouchPosX[i], SavedTouchPosY[i], 1, inputkind_finger);
                SavedTouchFlag = 0;
            }
            void mousePressEvent(QMouseEvent* event)
            {
                if(TabletPressed) return; // for tablet event
				if(!InputPressed[inputkind_mouse_right] && event->button() == Qt::LeftButton)
				{
					PostMouseEventsByWheelDone();
					InputPressed[inputkind_mouse_left] = true;
					PostMouseEvent(systouchtype_down, 0, event->x(), event->y(), 1, inputkind_mouse_left);
				}
				if(!InputPressed[inputkind_mouse_left] && event->button() == Qt::RightButton)
				{
					PostMouseEventsByWheelDone();
					InputPressed[inputkind_mouse_right] = true;
					SavedTouchAngle = 0;
					PostMouseEventByRotate(systouchtype_down, event->x(), event->y(), 1, SavedTouchAngle);
				}
            }
			void mouseMoveEvent(QMouseEvent* event)
            {
				if(InputPressed[inputkind_mouse_left] && (event->buttons() & Qt::LeftButton))
					PostMouseEvent(systouchtype_move, 0, event->x(), event->y(), 1, inputkind_mouse_left);
				if(InputPressed[inputkind_mouse_right] && (event->buttons() & Qt::RightButton))
					PostMouseEventByRotate(systouchtype_move, event->x(), event->y(), 1, SavedTouchAngle);
            }
            void mouseReleaseEvent(QMouseEvent* event)
            {
				if(InputPressed[inputkind_mouse_left] && event->button() == Qt::LeftButton)
				{
					InputPressed[inputkind_mouse_left] = false;
					PostMouseEvent(systouchtype_up, 0, event->x(), event->y(), 1, inputkind_mouse_left);
				}
				if(InputPressed[inputkind_mouse_right] && event->button() == Qt::RightButton)
				{
					InputPressed[inputkind_mouse_right] = false;
					PostMouseEventByRotate(systouchtype_up, event->x(), event->y(), 1, SavedTouchAngle);
				}
            }
            void keyPressEvent(QKeyEvent* event)
            {
                if(!event->isAutoRepeat())
                {
                    sysevent Event;
                    Event.type = syseventtype_key;
                    Event.key.type = syskeytype_down;
					#ifdef Q_OS_MACX
						const uint SCode = event->nativeScanCode();
						const uint VKey = event->nativeVirtualKey();
						const uint Key = event->key();
						Event.key.code = GetKeyKind((SCode)? VKey : Key);
					#else
						Event.key.code = GetKeyKind(event->key());
					#endif
                    BxScene::__AddEvent__(Event, syseventset_do_enable);
                }
                else event->ignore();
            }
            void keyReleaseEvent(QKeyEvent* event)
            {
                if(!event->isAutoRepeat())
                {
                    sysevent Event;
                    Event.type = syseventtype_key;
                    Event.key.type = syskeytype_up;
                    #ifdef Q_OS_MACX
						const uint SCode = event->nativeScanCode();
						const uint VKey = event->nativeVirtualKey();
						const uint Key = event->key();
						Event.key.code = GetKeyKind((SCode)? VKey : Key);
					#else
						Event.key.code = GetKeyKind(event->key());
					#endif
                    BxScene::__AddEvent__(Event, syseventset_need_enable);
                }
                else event->ignore();
            }
			void wheelEvent(QWheelEvent* event)
			{
                const int X = event->x();
				const int Y = event->y();
                const int Delta = event->delta();
                if(Delta == 0) return; // for tablet event

				if(InputPressed[inputkind_mouse_right])
				{
                    SavedTouchAngle = (SavedTouchAngle + 16 * Delta / 120 + 1024) % 1024;
                    PostMouseEventByRotate(systouchtype_move, X, Y, 1, SavedTouchAngle);
				}
				else if(!InputPressed[inputkind_mouse_left])
                {
					if(!InputPressed[inputkind_mouse_wheel])
					{
						InputPressed[inputkind_mouse_wheel] = true;
						RadiusForWheel = DClickRadius;
						PostMouseEvent(systouchtype_down, 0, X, Y, 1, inputkind_mouse_wheel, -RadiusForWheel);
						PostMouseEvent(systouchtype_down, 1, X, Y, 1, inputkind_mouse_wheel, +RadiusForWheel);
					}
					RadiusForWheel = BxUtilGlobal::Max(DClickRadiusMin,
                        RadiusForWheel + DClickRadiusSpeed * Delta / 120);
					PostMouseEvent(systouchtype_move, 0, X, Y, 1, inputkind_mouse_wheel, -RadiusForWheel);
					PostMouseEvent(systouchtype_move, 1, X, Y, 1, inputkind_mouse_wheel, +RadiusForWheel);
					WheelMoveTimer.start(20);
					WheelEndTimer.start(200);
				}
			}
            void tabletEvent(QTabletEvent *event)
            {
                if(BxCore::Wacom::IsExistDevice())
                    return;
                switch(event->type())
                {
                case QEvent::TabletPress:
                    TabletPressed = true;
                    PostMouseEvent(systouchtype_down, 0, event->posF().x(), event->posF().y(), event->pressure(), inputkind_wacom_pen);
                    break;
                case QEvent::TabletRelease:
                    TabletPressed = false;
                    PostMouseEvent(systouchtype_up, 0, event->posF().x(), event->posF().y(), event->pressure(), inputkind_wacom_pen);
                    break;
                case QEvent::TabletMove:
                    if(TabletPressed)
                        PostMouseEvent(systouchtype_move, 0, event->posF().x(), event->posF().y(), event->pressure(), inputkind_wacom_pen);
                    break;
                }
            }

		private:
			void PostMouseEvent(systouchtype type, uint id, float x, float y, float force, inputkind kind, int xadd = 0, int yadd = 0)
			{
				sysevent Event;
				Event.type = syseventtype_touch;
				Event.touch.type = type;
				Event.touch.id = id;
				const float fx = (x * GLSCALE - GetGUIMarginL() + xadd)
					* BxCore::Surface::GetWidth() / BxCore::Surface::GetWidthHW();
				const float fy = (y * GLSCALE - GetGUIMarginT() + yadd)
					* BxCore::Surface::GetHeight() / BxCore::Surface::GetHeightHW();
				Event.touch.x = (int) fx;
				Event.touch.y = (int) fy;
				Event.touch.kind = kind;
				Event.touch.fx = fx;
				Event.touch.fy = fy;
				Event.touch.force = force;
				BxScene::__AddEvent__(Event, (type == systouchtype_down)? syseventset_do_enable : syseventset_need_enable);
			}
			void PostMouseEventByRotate(systouchtype type, float x, float y, float force, int angle)
			{
				const int XAdd = FtoR(DClickRadius * BxUtil::Cos(angle));
				const int YAdd = FtoR(DClickRadius * BxUtil::Sin(angle));
				PostMouseEvent(type, 0, x, y, force, inputkind_mouse_right, -XAdd, -YAdd);
				PostMouseEvent(type, 1, x, y, force, inputkind_mouse_right, +XAdd, +YAdd);
			}
			void PostMouseEventsByWheelDone()
			{
				if(InputPressed[inputkind_mouse_wheel])
				{
					InputPressed[inputkind_mouse_wheel] = false;
					point CursorPos = BxCore::Simulator::GetCursorPos();
					point WindowPos = BxCore::Simulator::GetWindowPos();
					const int X = CursorPos.x - WindowPos.x;
					const int Y = CursorPos.y - WindowPos.y;
					PostMouseEvent(systouchtype_up, 0, X, Y, 1, inputkind_mouse_wheel, -RadiusForWheel);
					PostMouseEvent(systouchtype_up, 1, X, Y, 1, inputkind_mouse_wheel, +RadiusForWheel);
				}
			}

			#ifdef Q_OS_MACX
				keykind GetKeyKind(uint keycode)
				{
					switch(keycode)
					{
					case 0: return keykind_a;
					case 1: return keykind_s;
					case 2: return keykind_d;
					case 3: return keykind_f;
					case 4: return keykind_h;
					case 5: return keykind_g;
					case 6: return keykind_z;
					case 7: return keykind_x;
					case 8: return keykind_c;
					case 9: return keykind_v;
					case 10: break;
					case 11: return keykind_b;
					case 12: return keykind_q;
					case 13: return keykind_w;
					case 14: return keykind_e;
					case 15: return keykind_r;
					case 16: return keykind_y;
					case 17: return keykind_t;
					case 18: return keykind_1;
					case 19: return keykind_2;
					case 20: return keykind_3;
					case 21: return keykind_4;
					case 22: return keykind_6;
					case 23: return keykind_5;
					case 24: break;
					case 25: return keykind_9;
					case 26: return keykind_7;
					case 27: break;
					case 28: return keykind_8;
					case 29: return keykind_0;
					case 30: break;
					case 31: return keykind_o;
					case 32: return keykind_u;
					case 33: break;
					case 34: return keykind_i;
					case 35: return keykind_p;
					case 36: return keykind_enter;
					case 37: return keykind_l;
					case 38: return keykind_j;
					case 39: break;
					case 40: return keykind_k;
					case 41: case 42: case 43: case 44: break;
					case 45: return keykind_n;
					case 46: return keykind_m;
					case 47: break;
					case 48: return keykind_tab;
					case 49: return keykind_space;
					case 50: break;
					case 51: return keykind_bspace;
					case 52: break;
					case 53: return keykind_esc;
					}
					switch(keycode)
					{
					case 96: return keykind_f5;
					case 97: return keykind_f6;
					case 98: return keykind_f7;
					case 99: return keykind_f3;
					case 100: return keykind_f8;
					case 101: case 102: case 103: case 104: case 105: case 106:
					case 107: case 108: case 109: case 110: case 111: case 112:
					case 113: case 114: case 115: case 116: case 117: break;
					case 118: return keykind_f4;
					case 119: break;
					case 120: return keykind_f2;
					case 121: break;
					case 122: return keykind_f1;
					case 123: return keykind_left;
					case 124: return keykind_right;
					case 125: return keykind_down;
					case 126: return keykind_up;
					}
					switch(keycode)
					{
					case 0x1000020: return keykind_shift;
					case 0x1000021: break;
					case 0x1000022: return keykind_ctrl;
					}
					return keykind_null;
				}
			#else
				keykind GetKeyKind(uint keycode)
				{
					switch(keycode)
					{
					case Qt::Key_Escape: return keykind_esc;
					case Qt::Key_Tab: return keykind_tab;
					case Qt::Key_Backspace: return keykind_bspace;
					case Qt::Key_Return: return keykind_enter;
					case Qt::Key_Shift: return keykind_shift;
					case Qt::Key_Control: return keykind_ctrl;
					case Qt::Key_Space: return keykind_space;
					case Qt::Key_Left: return keykind_left;
					case Qt::Key_Up: return keykind_up;
					case Qt::Key_Right: return keykind_right;
					case Qt::Key_Down: return keykind_down;
					case Qt::Key_0: return keykind_0;
					case Qt::Key_1: return keykind_1;
					case Qt::Key_2: return keykind_2;
					case Qt::Key_3: return keykind_3;
					case Qt::Key_4: return keykind_4;
					case Qt::Key_5: return keykind_5;
					case Qt::Key_6: return keykind_6;
					case Qt::Key_7: return keykind_7;
					case Qt::Key_8: return keykind_8;
					case Qt::Key_9: return keykind_9;
					case Qt::Key_A: return keykind_a;
					case Qt::Key_B: return keykind_b;
					case Qt::Key_C: return keykind_c;
					case Qt::Key_D: return keykind_d;
					case Qt::Key_E: return keykind_e;
					case Qt::Key_F: return keykind_f;
					case Qt::Key_G: return keykind_g;
					case Qt::Key_H: return keykind_h;
					case Qt::Key_I: return keykind_i;
					case Qt::Key_J: return keykind_j;
					case Qt::Key_K: return keykind_k;
					case Qt::Key_L: return keykind_l;
					case Qt::Key_M: return keykind_m;
					case Qt::Key_N: return keykind_n;
					case Qt::Key_O: return keykind_o;
					case Qt::Key_P: return keykind_p;
					case Qt::Key_Q: return keykind_q;
					case Qt::Key_R: return keykind_r;
					case Qt::Key_S: return keykind_s;
					case Qt::Key_T: return keykind_t;
					case Qt::Key_U: return keykind_u;
					case Qt::Key_V: return keykind_v;
					case Qt::Key_W: return keykind_w;
					case Qt::Key_X: return keykind_x;
					case Qt::Key_Y: return keykind_y;
					case Qt::Key_Z: return keykind_z;
					case Qt::Key_F1: return keykind_f1;
					case Qt::Key_F2: return keykind_f2;
					case Qt::Key_F3: return keykind_f3;
					case Qt::Key_F4: return keykind_f4;
					case Qt::Key_F5: return keykind_f5;
					case Qt::Key_F6: return keykind_f6;
					case Qt::Key_F7: return keykind_f7;
					case Qt::Key_F8: return keykind_f8;
					case Qt::Key_Plus: return keykind_pad_plus;
					case Qt::Key_Minus: return keykind_pad_minus;
					case Qt::Key_Enter: return keykind_pad_enter;
					}
					return keykind_null;
				}
			#endif
        };
        /// @endcond

		/// @cond SECTION_NAME
		class ProgramClass : public QGLShaderProgram
		{
		public:
            ProgramClass(const QGLContext* context) : QGLShaderProgram(context), IsFirstBind(true)
			{global_data Data* Ref = BxNew(Data); This = Ref;}
			virtual ~ProgramClass() {}
		private:
			class Data
			{
			public:
				ProgramClass* Focus;
				Data() : Focus(nullptr) {}
				~Data() {}
			} *This;
			bool IsFirstBind;
		public:
			void Initialize()
			{
				BxCore::Main::GLWidget& Widget = BxCore::Main::GLWidget::Me();
                QGLShader* VShader = new QGLShader(QGLShader::Vertex, Widget.context());
				bool SuccessVShader = VShader->compileSourceCode(GetVertexScript());
				BxASSERT("BxCore::OpenGL2D<VShader의 컴파일이 실패하였습니다>", SuccessVShader);
                QGLShader* FShader = new QGLShader(QGLShader::Fragment, Widget.context());
				bool SuccessFShader = FShader->compileSourceCode(GetFragmentScript());
				BxASSERT("BxCore::OpenGL2D<FShader의 컴파일이 실패하였습니다>", SuccessFShader);
				addShader(VShader);
				addShader(FShader);
				InitAttributes();
			}
			QGLShaderProgram* SetFocus()
			{
				if(This->Focus != this)
				{
					if(This->Focus)
						This->Focus->Release();
					This->Focus = this;
					Bind();
					if(IsFirstBind)
					{
						IsFirstBind = false;
						InitUniforms();
					}
				}
				return This->Focus;
			}
		protected:
			virtual string GetVertexScript() = 0;
			virtual string GetFragmentScript() = 0;
			virtual void InitAttributes() = 0;
			virtual void InitUniforms() = 0;
			virtual void Bind() = 0;
			virtual void Release() = 0;
		};
		/// @endcond
    }

    namespace File
    {
        /// @cond SECTION_NAME
        class FileClass
        {
			QFile* File;
			bool Success;
		public:
			FileClass(const QString& fileName, const bool writable)
			{
				if(writable)
				{
					ValidRemFile(fileName);
                    File = new QFile(RootForWrite() + fileName);
                    Success = File->open(QFileDevice::WriteOnly);
                    BxTRACE("<>:FileOpen(<A>/<A>):<A>", BxARG((writable)? "W" : "R",
                        Success, (RootForWrite() + fileName).toLocal8Bit().constData()));
				}
				else
				{
                    QString FilePath = "";
                    if(QFileInfo(RootForWrite() + fileName).exists())
                        FilePath = RootForWrite() + fileName;
					#ifdef WIN32
						else if(QFileInfo(RootForSystem() + fileName).exists())
                            FilePath = RootForSystem() + fileName;
					#endif
                    else FilePath = RootForRead() + fileName;
                    File = new QFile(FilePath);
                    Success = File->open(QFileDevice::ReadOnly);
                    BxTRACE("<>:FileOpen(<A>/<A>):<A>", BxARG((writable)? "W" : "R",
                        Success, FilePath.toLocal8Bit().constData()));
				}
			}
            ~FileClass() {delete File;}
		public:
			bool IsSuccess() {return Success;}
			QFile* operator->() {return File;}
        public:
            static const QString& RootForWrite()
            {
                return ValidRemRoot();
            }
            static const QString& RootForRead()
            {
                static QString Result = "";
                if(0 < Result.length()) return Result;
                #ifdef WIN32
                    static QString Root = "../resource/assets/";
                #elif defined(Q_OS_MACX)
                    static QString Root = QCoreApplication::applicationDirPath() + "/../../assets/";
                #elif ANDROID
                    static QString Root = "assets:/";
                #elif TARGET_OS_IPHONE == 1
                    static QString Root = QCoreApplication::applicationDirPath() + "/assets/";
                #else
                    static QString Root = "../resource/assets/";
                #endif
                return Result = Root;
            }
            static const QString& RootForSystem()
            {
                static QString Root = "../../Bx2D/assets/";
                return Root;
            }
            static bool IsFile(const QString& fileName)
            {
                QFileInfo InfoW(RootForWrite() + fileName);
				if(InfoW.exists()) return InfoW.isFile();
				#ifdef WIN32
					QFileInfo InfoS(RootForSystem() + fileName);
					if(InfoS.exists()) return InfoS.isFile();
				#endif
                return QFileInfo(RootForRead() + fileName).isFile();
            }
            static bool Remove(const QString& fileName)
            {
                return QFile::remove(RootForWrite() + fileName);
            }
            static bool Rename(const QString& oldName, const QString& newName)
            {
				ValidRemFile(oldName);
                return QFile::rename(RootForWrite() + oldName, RootForWrite() + newName);
            }
            bool CopyToCamera(const QString& newName)
            {
                QString FilePath = ValidCameraRoot() + newName;
                if(QFileInfo(FilePath).exists())
                    QFile::remove(FilePath);
                bool Result = File->copy(FilePath);
                BxTRACE("<>:CopyToCamera(<A>):<A>", BxARG(Result, FilePath.toLocal8Bit().constData()));
                return Result;
            }
		private:
            static void ValidRemFile(const QString& fileName)
            {
                if(!QFileInfo(RootForWrite() + fileName).exists())
                {
                    #ifdef WIN32
                        if(QFileInfo(RootForSystem() + fileName).exists())
                            QFile::copy(RootForSystem() + fileName, RootForWrite() + fileName);
                        else
                    #endif
                    if(QFileInfo(RootForRead() + fileName).exists())
                        QFile::copy(RootForRead() + fileName, RootForWrite() + fileName);
                }
            }
            static const QString& ValidRemRoot()
            {
                static QString Result = "";
                if(0 < Result.length()) return Result;
                #ifdef WIN32
                    QString Root = "../resource";
                #else
                    QString Root = QStandardPaths::standardLocations(QStandardPaths::DataLocation).value(0);
                #endif
                if(!QFileInfo(Root).exists()) QDir().mkdir(Root);
                Root += "/assets-rem";
                if(!QFileInfo(Root).exists()) QDir().mkdir(Root);
                return Result = Root + "/";
            }
            static QString ValidCameraRoot()
            {
                static QString Result = "";
                if(0 < Result.length()) return Result;
                #if defined(WIN32) || defined(Q_OS_MACX)
                    QString Root = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).value(0);
                #else
                    QString Root = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation).value(0);
                    if(!QFileInfo(Root).exists()) QDir().mkdir(Root);
                    Root += "/DCIM";
                #endif
                if(!QFileInfo(Root).exists()) QDir().mkdir(Root);
				Root += "/Camera";
                if(!QFileInfo(Root).exists()) QDir().mkdir(Root);
                return Result = Root + "/";
            }
        };
        /// @endcond

        /// @cond SECTION_NAME
        class FileInfoClass
        {
			QFileInfo* FileInfo;
        public:
            FileInfoClass(const QString& fileName)
			{
                if(QFileInfo(FileClass::RootForWrite() + fileName).exists())
                    FileInfo = new QFileInfo(FileClass::RootForWrite() + fileName);
				#ifdef WIN32
					else if(QFileInfo(FileClass::RootForSystem() + fileName).exists())
						FileInfo = new QFileInfo(FileClass::RootForSystem() + fileName);
				#endif
                else FileInfo = new QFileInfo(FileClass::RootForRead() + fileName);
			}
            ~FileInfoClass() {delete FileInfo;}
		public:
			QFileInfo* operator->() {return FileInfo;}
        };
        /// @endcond

        /// @cond SECTION_NAME
        class DirClass
        {
        public:
			static void EntryList(const QString& pathName, QStringList& fileNames)
			{
				// 체크할 것!!!
                QStringList ListW = QDir(FileClass::RootForWrite() + pathName).entryList
                    (QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
				for(int i = 0, iend = ListW.size(); i < iend; ++i)
                    if(fileNames.indexOf(ListW.at(i)) == -1)
                    {
                        BxTRACE("<>:EntryList(W) : <A>", BxARG(ListW.at(i).toLocal8Bit().constData()));
                        fileNames << ListW.at(i);
                    }
				#ifdef WIN32
					QStringList ListS = QDir(FileClass::RootForSystem() + pathName).entryList
						(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
					for(int i = 0, iend = ListS.size(); i < iend; ++i)
                        if(fileNames.indexOf(ListS.at(i)) == -1)
                        {
                            BxTRACE("<>:EntryList(S) : <A>", BxARG(ListS.at(i).toLocal8Bit().constData()));
                            fileNames << ListS.at(i);
                        }
				#endif
                QStringList ListR = QDir(FileClass::RootForRead() + pathName).entryList
                    (QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
				for(int i = 0, iend = ListR.size(); i < iend; ++i)
                    if(fileNames.indexOf(ListR.at(i)) == -1)
                    {
                        BxTRACE("<>:EntryList(R) : <A>", BxARG(ListR.at(i).toLocal8Bit().constData()));
                        fileNames << ListR.at(i);
                    }
			}
            static bool Mkdir(const QString& pathName) {return QDir().mkdir(FileClass::RootForWrite() + pathName);}
            static bool Rmdir(const QString& pathName) {return QDir().rmdir(FileClass::RootForWrite() + pathName);}
        };
        /// @endcond
    }

    namespace Surface
    {
        /// @cond SECTION_NAME
        class SurfaceSingle
        {
        public:
            void SetRenderMode(rendermode mode)
            {
                if(This->RenderMode != mode)
                {
                    #ifdef __BX_OPENGL
                        if(This->RenderMode != rendermode_null)
                            BxCore::OpenGL2D::Flush();
                        if(mode == rendermode_2d)
                        {
							glDisable(GL_DEPTH_TEST);
                            glDisable(GL_CULL_FACE);
							glEnable(GL_SCISSOR_TEST);
							glEnable(GL_TEXTURE_2D);
							glEnable(GL_BLEND);
							glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                        }
                        else if(mode == rendermode_3d)
                        {
                            glEnable(GL_DEPTH_TEST);
                            glEnable(GL_CULL_FACE);
							glEnable(GL_SCISSOR_TEST);
							glEnable(GL_TEXTURE_2D);
							glEnable(GL_BLEND);
							glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
							glClear(GL_DEPTH_BUFFER_BIT);
                        }
                    #endif
                    This->RenderMode = mode;
                }
            }
            bool GetMarginEnable() {return This->MarginEnable;}
            void SetMarginEnable(bool enable) {This->MarginEnable = enable;}
            int GetWidthHW() {return BxCore::Main::GLWidget::Me().width() * GLSCALE;}
            int GetHeightHW() {return BxCore::Main::GLWidget::Me().height() * GLSCALE;}
        public:
            SurfaceSingle() {global_data Data* Ref = BxNew(Data); This = Ref;}
        private:
            class Data
            {
            public:
                rendermode RenderMode;
                bool MarginEnable;
                Data() : RenderMode(rendermode_null), MarginEnable(true) {}
                ~Data() {}
            } *This;
        };
        /// @endcond
    }

	namespace Socket
	{
		/// @cond SECTION_NAME
		class TCPData : public QTcpSocket
		{
			Q_OBJECT
		private:
			socketstate State;
			int RecvLength;
		private slots:
			void OnConnected()
			{
				State = socketstate_connected;
			}
			void OnDisconnected()
			{
				State = socketstate_disconnected;
			}
			void OnReadyRead()
			{
				QTcpSocket* Peer = (QTcpSocket*) sender();
				RecvLength = (int) Peer->bytesAvailable();
			}
			void OnError()
			{
				State = socketstate_null;
			}
		public:
			TCPData() : State(socketstate_null)
			{
				connect(this, SIGNAL(connected()), this, SLOT(OnConnected()));
				connect(this, SIGNAL(disconnected()), this, SLOT(OnDisconnected()));
				connect(this, SIGNAL(readyRead()), this, SLOT(OnReadyRead()));
				connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnError()));
				ResetClient();
			}
			virtual ~TCPData()
			{
				Disconnect(true);
			}
			inline socketstate GetState()
			{
				return State;
			}
			inline void SetState(socketstate state)
			{
				State = state;
			}
			inline int GetRecvLength()
			{
				return RecvLength;
			}
			inline void SubRecvLength(int length)
			{
				if(0 < length && (RecvLength -= length) == 0)
					connect(this, SIGNAL(readyRead()), this, SLOT(OnReadyRead()));
			}
			inline void ResetClient()
			{
				Disconnect(true);
				State = socketstate_created;
				RecvLength = 0;
			}
			inline void Disconnect(bool dowait)
			{
				disconnectFromHost();
				if(dowait && !waitForDisconnected())
					State = socketstate_null;
			}
		};
		/// @endcond
	}

	namespace Server
	{
		/// @cond SECTION_NAME
		class TCPPeerData : public QObjectUserData
		{
		public:
			const int ID;
			huge PacketNeeds;
		public:
			TCPPeerData() : ID(MakeID()) {PacketNeeds = 0;}
			virtual ~TCPPeerData() {}
		public:
			static uint ClassID()
			{static uint _ = QObject::registerUserData(); return _;}
			static int MakeID() {static int _ = -1; return ++_;}
		};
		/// @endcond

		/// @cond SECTION_NAME
		class TCPPacket
		{
		public:
			const peerpacketkind Kind;
			const int PeerID;
			const huge BufferSize;
			byte* Buffer;
		public:
			TCPPacket(peerpacketkind kind, int peerid, huge buffersize)
				: Kind(kind), PeerID(peerid), BufferSize(buffersize) {Buffer = new byte[buffersize];}
			~TCPPacket() {delete[] Buffer;}
		public:
			void DestroyMe() {delete this;}
		};
		/// @endcond

		/// @cond SECTION_NAME
		class TCPAgent : public QTcpServer
		{
			Q_OBJECT
		private:
			bool UsingSizeField;
			BxVarMap<QTcpSocket*, 0, true> Peers;
			BxVarQueue<TCPPacket, true> PacketQueue;
			TCPPacket* LastPacket;
		private slots:
			void acceptPeer()
			{
				QTcpSocket* Peer = nextPendingConnection();
				TCPPeerData* NewData = new TCPPeerData();
				Peers[NewData->ID] = Peer;
				Peer->setUserData(TCPPeerData::ClassID(), NewData);
				PacketQueue.Enqueue(new TCPPacket(peerpacketkind_entrance, NewData->ID, 0));

				if(!UsingSizeField) connect(Peer, SIGNAL(readyRead()), this, SLOT(readyPeer()));
				else connect(Peer, SIGNAL(readyRead()), this, SLOT(readyPeerWithSizeField()));
				connect(Peer, SIGNAL(error(QAbstractSocket::SocketError)),
					this, SLOT(errorPeer(QAbstractSocket::SocketError)));
			}
			void readyPeer()
			{
				QTcpSocket* Peer = (QTcpSocket*) sender();
				TCPPeerData* Data = (TCPPeerData*) Peer->userData(TCPPeerData::ClassID());
				huge PacketSize = Peer->bytesAvailable();

				TCPPacket* NewPacket = new TCPPacket(peerpacketkind_message, Data->ID, PacketSize);
				Peer->read((char*) NewPacket->Buffer, PacketSize);
				PacketQueue.Enqueue(NewPacket);

				connect(Peer, SIGNAL(readyRead()), this, SLOT(readyPeer()));
			}
			void readyPeerWithSizeField()
			{
				QTcpSocket* Peer = (QTcpSocket*) sender();
				TCPPeerData* Data = (TCPPeerData*) Peer->userData(TCPPeerData::ClassID());
				huge PacketSize = Peer->bytesAvailable();

				while(0 < PacketSize)
				{
					if(Data->PacketNeeds == 0)
					{
						if(4 <= PacketSize)
						{
							PacketSize -= 4;
							int GetPacketSize = 0;
							Peer->read((char*) &GetPacketSize, 4);
							Data->PacketNeeds = GetPacketSize;
						}
						else
						{
							connect(Peer, SIGNAL(readyRead()), this, SLOT(readyPeerWithSizeField()));
							break;
						}
					}
					if(0 < Data->PacketNeeds)
					{
						if(Data->PacketNeeds <= PacketSize)
						{
							PacketSize -= Data->PacketNeeds;
							TCPPacket* NewPacket = new TCPPacket(peerpacketkind_message, Data->ID, Data->PacketNeeds);
							Peer->read((char*) NewPacket->Buffer, Data->PacketNeeds);
							PacketQueue.Enqueue(NewPacket);
							Data->PacketNeeds = 0;
						}
						else
						{
							connect(Peer, SIGNAL(readyRead()), this, SLOT(readyPeerWithSizeField()));
							break;
						}
					}
				}
			}
			void errorPeer(QAbstractSocket::SocketError error)
			{
				QTcpSocket* Peer = (QTcpSocket*) sender();
				TCPPeerData* Data = (TCPPeerData*) Peer->userData(TCPPeerData::ClassID());
				Peers.Remove(Data->ID);
				PacketQueue.Enqueue(new TCPPacket((error == QAbstractSocket::RemoteHostClosedError)?
					peerpacketkind_leaved : peerpacketkind_kicked, Data->ID, 0));
			}
		public:
			TCPAgent(bool sizefield)
			{
				UsingSizeField = sizefield;
				LastPacket = new TCPPacket(peerpacketkind_null, -1, 0);
				connect(this, SIGNAL(newConnection()), this, SLOT(acceptPeer()));
			}
			virtual ~TCPAgent()
			{
				for(TCPPacket* UnusedPacket = nullptr; UnusedPacket = PacketQueue.Dequeue();)
					UnusedPacket->DestroyMe();
				delete LastPacket;
			}
		public:
			bool TryPacket()
			{
				TCPPacket* PopPacket = PacketQueue.Dequeue();
				if(!PopPacket) return false;
				delete LastPacket;
				LastPacket = PopPacket;
				return true;
			}
			TCPPacket* GetLastPacket()
			{
				return LastPacket;
			}
			bool SendPacket(int peerid, const void* buffer, huge buffersize)
			{
				QTcpSocket* Peer = Peers[peerid];
				if(!Peer)
				{
					Peers.Remove(peerid);
					return false;
				}
				const huge SendedSize = Peer->write((string) buffer, buffersize);
				return (SendedSize == buffersize);
			}
		};
		/// @endcond
	}

	namespace Bluetooth
	{
		/// @cond SECTION_NAME
        #if defined(WIN32)
            class _DeviceAgent : public QBluetoothDeviceDiscoveryAgent
            {
                Q_OBJECT
            public:
                _DeviceAgent()
                {
                    connect(this, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
                        this, SLOT(addDevice(const QBluetoothDeviceInfo&)));
                    connect(this, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
                        this, SLOT(deviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));
                    connect(this, SIGNAL(finished()), this, SLOT(deviceScanFinished()));

                }
                virtual ~_DeviceAgent() {}
            private slots:
                void addDevice(const QBluetoothDeviceInfo& info)
                {
                    BxASSERT(BxCore::Util::Print("<>:##-addDevice[<A>]", BxARG(info.name().toLocal8Bit().constData())), false);
                }
                void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
                {
                    switch(error)
                    {
                    case QBluetoothDeviceDiscoveryAgent::NoError:
                        BxASSERT("##-deviceScanError:NoError", false);
                        break;
                    case QBluetoothDeviceDiscoveryAgent::InputOutputError:
                        BxASSERT("##-deviceScanError:InputOutputError", false);
                        break;
                    case QBluetoothDeviceDiscoveryAgent::PoweredOffError:
                        BxASSERT("##-deviceScanError:PoweredOffError", false);
                        break;
                    case QBluetoothDeviceDiscoveryAgent::InvalidBluetoothAdapterError:
                        BxASSERT("##-deviceScanError:InvalidBluetoothAdapterError", false);
                        break;
                    case QBluetoothDeviceDiscoveryAgent::UnknownError:
                        BxASSERT("##-deviceScanError:UnknownError", false);
                        break;
                    }
                }
                void deviceScanFinished()
                {
                    BxASSERT("##-deviceScanFinished", false);
                }
            };
            SINGLETON_CLASS(_DeviceAgent, DeviceAgent);
        #endif
		/// @endcond
	}

	namespace Font
    {
		/// @cond SECTION_NAME
		class TextClass
		{
			private: enum {EndMark = 1, SafeDummy = 2};

			public: wchar operator[](int index) const
			{
				BxASSERT("BxCore::Font<Text가 nullptr입니다>", Text);
				BxASSERT("BxCore::Font<Text의 한계를 벗어난 접근입니다>", 0 <= index && index < Length);
				return Text[index];
			}

			public: int GetLength() const
			{
				return Length;
			}

			public: int& GetSavedWidth(int uniq, int offset, int length) const
			{
				if(SavedUniq != uniq || SavedOffset != offset || SavedLength != length)
				{
					SavedUniq = uniq;
					SavedOffset = offset;
					SavedLength = length;
					SavedWidth = -1;
				}
				return SavedWidth;
			}

			// 멤버
			public: TextClass(string str, textencoding encoding)
				: ReferOnly(false), Text(nullptr), Length(0), SavedUniq(0), SavedOffset(0), SavedLength(0), SavedWidth(0)
			{
				BxASSERT("BxCore::Font<str이 nullptr입니다>", str);
				if(encoding == textencoding_cp949)
				{
					const int StrLength = BxUtilGlobal::StrLenEx(str); // str은 개행문자로 끝날 지도
					Text = (wstring_rw) BxAlloc(sizeof(wchar) * (StrLength + EndMark + SafeDummy));
					Length = BxUtil::CP949ToUTF16(str, StrLength, Text, StrLength); // EndMark 처리됨
				}
				else if(encoding == textencoding_utf8)
				{
					const int StrLength = BxUtilGlobal::StrLenEx(str); // str은 개행문자로 끝날 지도
					Length = BxUtil::GetLengthForUTF16(str, StrLength);
					Text = (wstring_rw) BxAlloc(sizeof(wchar) * (Length + EndMark + SafeDummy));
					BxUtil::UTF8ToUTF16(str, StrLength, Text); // EndMark 처리됨
				}
				else BxASSERT("BxCore::Font<현재 지원하지 않는 인코딩입니다>", false);
			}
			public: TextClass(wstring wstr, bool referonly)
				: ReferOnly(referonly), Text(nullptr), Length(0), SavedUniq(0), SavedOffset(0), SavedLength(0), SavedWidth(0)
			{
				BxASSERT("BxCore::Font<wstr이 nullptr입니다>", wstr);
				Length = BxUtilGlobal::StrLenEx(wstr); // wstr은 개행문자로 끝날 지도
				if(ReferOnly) Text = (wstring_rw) wstr;
				else
				{
					Text = (wstring_rw) BxAlloc(sizeof(wchar) * (Length + EndMark));
					BxCore::Util::MemCpy(Text, wstr, sizeof(wchar) * Length);
					Text[Length] = _W2('\0'); // EndMark 처리가 필요
				}
			}
			public: ~TextClass()
			{
				if(!ReferOnly) BxFree(Text);
			}
			private: bool ReferOnly;
			private: wstring_rw Text;
			private: int Length;
			private: mutable int SavedUniq;
			private: mutable int SavedOffset;
			private: mutable int SavedLength;
			private: mutable int SavedWidth;
		};
		/// @endcond

		/// @cond SECTION_NAME
		class GraphClass
		{
			public: struct CharData
            {
				wchar Code;
                GLuint TextureID;
				int CellWidth;
				int CellHeight;
				int CellHx;
				int CellHy;
                GLfloat s[2], t[2];
				int SavedWidth;
            };
			public: class RenderData
			{
				public: enum {TextureSize = 256};

				public: CharData& GetCharData(wchar c)
				{
					if(!Chars.contains(c))
					{
						CharData& CurData = Chars[c];
						CurData.Code = c;
						CurData.TextureID = 0;
						CurData.SavedWidth = -1;
						return CurData;
					}
					return Chars[c];
				}

				public: void BindTextureForCharData(const GraphClass* graph, CharData& data)
				{
					const byte* NewBitmap = BxCore::AddOn::TTFToBMP(graph->TTF, SavedHeight, (uint) data.Code);
					const int BitmapWidth = BxCore::AddOn::GetBMPWidth(NewBitmap);
					const int BitmapHeight = BxCore::AddOn::GetBMPHeight(NewBitmap);
					const int BitmapHx = BxCore::AddOn::GetBMPParam1(NewBitmap);
					const int BitmapHy = BxCore::AddOn::GetBMPParam2(NewBitmap);
					const byte* BitmapBits = (const byte*) BxCore::AddOn::GetBMPBits(NewBitmap);

					const int SpaceGap = 1;
					const int SpaceWidth = BitmapWidth + SpaceGap;
					const int SpaceHeight = BitmapHeight + SpaceGap;
					if(TextureSize < LastOffsetX + SpaceWidth)
					{
						LastOffsetX = SpaceGap;
						LastOffsetY += LastMaxHeight;
						LastMaxHeight = 0;
					}
					LastMaxHeight = (LastMaxHeight < SpaceHeight)? SpaceHeight : LastMaxHeight;
					if(TextureSize < LastOffsetY + LastMaxHeight)
					{
						LastOffsetX = SpaceGap;
						LastOffsetY = SpaceGap;
						AddTexture();
					}

					GLuint LastTexture = Textures.last();
					BxCore::Main::GLWidget::Me().BindTexture(LastTexture);
					glTexSubImage2D(GL_TEXTURE_2D, 0, LastOffsetX, LastOffsetY,
						BitmapWidth, BitmapHeight, GL_RGBA, GL_UNSIGNED_BYTE, BitmapBits);
					BxCore::AddOn::Release(NewBitmap);

					data.TextureID = LastTexture;
					data.CellWidth = BitmapWidth;
					data.CellHeight = BitmapHeight;
					data.CellHx = BitmapHx;
					data.CellHy = BitmapHy;
					data.s[0] = ((GLfloat) LastOffsetX) / TextureSize;
					data.t[0] = ((GLfloat) LastOffsetY) / TextureSize;
					data.s[1] = ((GLfloat) LastOffsetX + BitmapWidth) / TextureSize;
					data.t[1] = ((GLfloat) LastOffsetY + BitmapHeight) / TextureSize;
					LastOffsetX += SpaceWidth;
				}

				// 멤버
				friend class GraphClass;
				public: RenderData()
				{
					// 초기 BindTextureForCharData호출시 AddTexture호출을 위한 트릭
					LastOffsetX = TextureSize;
					LastOffsetY = TextureSize;
					LastMaxHeight = 0;
					SavedHeight = 0;
				}
				public: ~RenderData()
				{
					foreach(GLuint Texture, Textures)
						glDeleteTextures(1, &Texture);
				}
				private: QHash<unsigned short, CharData> Chars;
				private: QList<GLuint> Textures;
				private: int LastOffsetX;
				private: int LastOffsetY;
				private: int LastMaxHeight;
				private: int SavedHeight;

				// 비공개부
				private: void AddTexture()
				{
					QImage NewImage(TextureSize, TextureSize, QImage::Format_ARGB32);
					NewImage.fill(Qt::transparent);
					NewImage = QGLWidget::convertToGLFormat(NewImage); // 상하반전 및 기타변환

					GLuint NewTexture;
					glGenTextures(1, &NewTexture);
					BxCore::Main::GLWidget::Me().BindTexture(NewTexture);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TextureSize, TextureSize,
						0, GL_RGBA, GL_UNSIGNED_BYTE, NewImage.bits());
					Textures += NewTexture;
				}
			};

			public: bool IsCreated()
			{
				return (TTF != nullptr);
			}
			public: bool Create(const byte* buffer, int buffersize)
			{
				global_data ushort UniqCounter = 0;
				BxASSERT("BxCore::Font<이미 생성된 TTF을 다시 생성하려 합니다>", !TTF);
				if(!TTF)
				{
					TTF = BxCore::AddOn::CreateTTF(buffer, buffersize);
					BxASSERT("BxCore::Font<해당 버퍼로 TTF의 생성에 실패하였습니다>", TTF);
					Uniq = ++UniqCounter;
					if(!Datas.isEmpty())
						Datas.clear();
				}
				return (TTF != nullptr);
			}
			public: int CalcUniq(int height) const
			{
				return (Uniq << 15) ^ height;
			}
			public: int CalcWidth(int height, wchar code) const
			{
				int Width = 0;
				BxCore::AddOn::TTFToINFO(TTF, height, (uint) code, &Width);
				return Width;
			}
			public: int CalcAscent(int height) const
			{
				int Ascent = 0;
				BxCore::AddOn::TTFToINFO(TTF, height, 0, nullptr, &Ascent);
				return Ascent;
			}
			public: RenderData& GetRenderData(int height) const
			{
				if(!Datas.contains(height))
				{
					RenderData& CurRender = Datas[height];
					CurRender.SavedHeight = height;
					return CurRender;
				}
				return Datas[height];
			}

			// 멤버
			public: GraphClass() : TTF(nullptr), Uniq(0) {}
			public: ~GraphClass() {BxCore::AddOn::ReleaseTTF(TTF);}
			private: id_ttf TTF;
			private: int Uniq;
			private: mutable QHash<unsigned short, RenderData> Datas;
		};
		/// @endcond

		/// @cond SECTION_NAME
		class FontClass
		{
			private: struct DrawData
            {
				GraphClass::CharData* Data;
				QColor Color;
				float x[4], y[4];
				float x_debug[4], y_debug[4];
            };

			public: void SetOption(color_x888 color, byte opacity, textalign align)
			{
				Color = color;
				Opacity = opacity;
				Align = align;
			}
			public: void SetRotate(int angle1024)
			{
				Angle1024 = angle1024;
			}
			public: void SetJustify(int column)
			{
				Column = column;
			}
			public: int GetWidth(const TextClass& text, int offset, int length)
			{
				int& SavedWidth = text.GetSavedWidth(Graph->CalcUniq(SavedHeight), offset, length);
				if(SavedWidth == -1)
				{
					int WidthSum = 0;
					for(int i = offset, iend = i + length; i < iend; ++i)
						WidthSum += Graph->CalcWidth(SavedHeight, text[i]);
					SavedWidth = WidthSum;
				}
				return SavedWidth;
			}
			public: int GetHeight()
			{
				return SavedHeight;
			}
			public: int GetAscent()
			{
				return Graph->CalcAscent(SavedHeight);
			}
			public: int GetLength(int column, const TextClass& text, int offset, int* width)
			{
				return GetLengthByLetter(column, text, offset, width); ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			}
			public: int GetLengthByLetter(int column, const TextClass& text, int offset, int* width)
			{
				int WidthSum = 0, i = -1;
				while(++i < 40960) // For safety
				{
					const wchar CurChar = text[offset + i];
					if(CurChar == _W2('\0')) break;
					if(CurChar == _W2('\r')) break;
					if(CurChar == _W2('\n')) break;
					const int CurWidth = Graph->CalcWidth(SavedHeight, CurChar);
					if(column < WidthSum + CurWidth) break;
					WidthSum += CurWidth;
				}
				if(width) *width = WidthSum;
				return i;
			}
			public: point DrawRequest(point pos, const TextClass& text, int offset, int length)
			{
				int PosX = 0;
				switch(Align)
				{
				case textalign_left_top: case textalign_left_middle: case textalign_left_ascent: case textalign_left_bottom:
					PosX = pos.x; break;
				case textalign_center_top: case textalign_center_middle: case textalign_center_ascent: case textalign_center_bottom:
					PosX = pos.x - GetWidth(text, offset, length) / 2; break;
				case textalign_right_top: case textalign_right_middle: case textalign_right_ascent: case textalign_right_bottom:
					PosX = pos.x - GetWidth(text, offset, length); break;
				}
				int PosY = 0;
				switch(Align)
				{
				case textalign_left_top: case textalign_center_top: case textalign_right_top:
					PosY = pos.y; break;
				case textalign_left_middle: case textalign_center_middle: case textalign_right_middle:
					PosY = pos.y - SavedHeight / 2; break;
				case textalign_left_ascent: case textalign_center_ascent: case textalign_right_ascent:
					PosY = pos.y - GetAscent(); break;
				case textalign_left_bottom: case textalign_center_bottom: case textalign_right_bottom:
					PosY = pos.y - SavedHeight; break;
				}

				const int X = BxCore::Main::GetGUIMarginL() + PosX;
				const int Y = BxCore::Main::GetGUIMarginT() + PosY;
				const int WidthHW = BxCore::Surface::SurfaceSingle().GetWidthHW(); /////////////////// 클리핑영역 현실적으로 /////////////////////////////////////////
                const int HeightHW = BxCore::Surface::SurfaceSingle().GetHeightHW();
                const int HalfWidth = WidthHW / 2;
                const int HalfHeight = HeightHW / 2;
                const float DeWidth = 1.0f / WidthHW;
                const float DeHeight = 1.0f / HeightHW;

				// 수직클리핑
				point Result = XY(X, Y + SavedHeight);
				const int YBegin = Y - HalfHeight;
				const int YEnd = YBegin + SavedHeight;
				if(YEnd <= -HalfHeight || HalfHeight <= YBegin)
				{
					Result.x += GetWidth(text, offset, length);
					return Result;
				}

				// 수집과정
				GraphClass::RenderData& CurRender = Graph->GetRenderData(SavedHeight);
				for(int i = offset, iend = i + length; i < iend; ++i)
                {
					GraphClass::CharData& CurData = CurRender.GetCharData(text[i]);
					if(CurData.SavedWidth == -1)
						CurData.SavedWidth = Graph->CalcWidth(SavedHeight, CurData.Code);

					// 수평클리핑
					const int XBegin = Result.x - HalfWidth;
					const int XEnd = XBegin + CurData.SavedWidth;
					if(-HalfWidth < XEnd && XBegin < HalfWidth)
					{
						if(CurData.TextureID == 0)
							CurRender.BindTextureForCharData(Graph, CurData);
						DrawVector* DrawList = DrawListMap.Access((int) CurData.TextureID);
						if(!DrawList)
						{
							DrawList = &DrawListMap[(int) CurData.TextureID];
							DrawIDList[LAST] = CurData.TextureID;
						}

						// 출력예약
						DrawData* Data = &(*DrawList)[LAST];
						Data->Data = &CurData;
						Data->Color = QColor((Color >> 16) & 0xFF, (Color >> 8) & 0xFF, Color & 0xFF, Opacity);
						const int X1 = XBegin + CurData.CellHx;
						const int Y1 = YBegin + CurData.CellHy;
						const int X2 = X1 + CurData.CellWidth;
						const int Y2 = Y1 + CurData.CellHeight;
						Data->x[0] = X1 * DeWidth; Data->y[0] = Y1 * DeHeight;
						Data->x[1] = X1 * DeWidth; Data->y[1] = Y2 * DeHeight;
						Data->x[2] = X2 * DeWidth; Data->y[2] = Y2 * DeHeight;
						Data->x[3] = X2 * DeWidth; Data->y[3] = Y1 * DeHeight;
						// 폰트디버그
						if(BxCore::System::IsEnableFontDebug())
						{
							Data->x_debug[0] = XBegin * DeWidth; Data->y_debug[0] = YBegin * DeHeight;
							Data->x_debug[1] = XBegin * DeWidth; Data->y_debug[1] = YEnd * DeHeight;
							Data->x_debug[2] = (XEnd - 1) * DeWidth; Data->y_debug[2] = YEnd * DeHeight;
							Data->x_debug[3] = (XEnd - 1) * DeWidth; Data->y_debug[3] = YBegin * DeHeight;
						}
					}
					Result.x += CurData.SavedWidth;
                }
				return Result;
			}
			public: void RequestFlush(QGLShaderProgram* Program)
			{
				BxCore::Surface::SurfaceSingle().SetRenderMode(rendermode_2d);
				QColor OldColor(0, 0, 0, 0);
				for(int i = 0, iend = DrawIDList.Length(); i < iend; ++i)
                {
					const GLuint TextureID = DrawIDList[i];
					BxCore::Main::GLWidget::Me().BindTexture(TextureID);
					DrawVector& DrawList = *DrawListMap.Access((int) TextureID);
					for(int j = 0, jend = DrawList.Length(); j < jend; ++j)
					{
						DrawData& Data = *DrawList.Access(j);
						if(OldColor != Data.Color)
							Program->setUniformValue("color", OldColor = Data.Color);
						QVector3D Vertex[4] = {
							QVector3D(Data.x[0], Data.y[0], 0),
							QVector3D(Data.x[1], Data.y[1], 0),
							QVector3D(Data.x[2], Data.y[2], 0),
							QVector3D(Data.x[3], Data.y[3], 0)};
						Program->setAttributeArray(0, Vertex);
						QVector2D UV[4] = {
							QVector2D(Data.Data->s[0], Data.Data->t[1]),
							QVector2D(Data.Data->s[0], Data.Data->t[0]),
							QVector2D(Data.Data->s[1], Data.Data->t[0]),
							QVector2D(Data.Data->s[1], Data.Data->t[1])};
						Program->setAttributeArray(1, UV);
						glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
					}
                }

				// 출력요청 클리어
				DrawListMap.Reset();
				DrawIDList.Reset();
			}
			public: void CheckRequestDebug(QGLShaderProgram* ProgramDebug)
			{
				BxCore::Surface::SurfaceSingle().SetRenderMode(rendermode_2d);
				QColor BGColor = QColor(255, 0, 255, 128);
				ProgramDebug->setUniformValue("color", BGColor);
				for(int i = 0, iend = DrawIDList.Length(); i < iend; ++i)
                {
					const GLuint TextureID = DrawIDList[i];
					DrawVector& DrawList = *DrawListMap.Access((int) TextureID);
					for(int j = 0, jend = DrawList.Length(); j < jend; ++j)
					{
						DrawData& Data = *DrawList.Access(j);
						QVector3D Vertex[4] = {
							QVector3D(Data.x_debug[0], Data.y_debug[0], 0),
							QVector3D(Data.x_debug[1], Data.y_debug[1], 0),
							QVector3D(Data.x_debug[2], Data.y_debug[2], 0),
							QVector3D(Data.x_debug[3], Data.y_debug[3], 0)};
						ProgramDebug->setAttributeArray(0, Vertex);
						glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
					}
                }
			}

			// 멤버
			public: FontClass(GraphClass* graph, int height) : Graph(graph), SavedHeight(height)
			{
				Color = 0;
				Opacity = 0xFF;
				Align = textalign_left_top;
				Angle1024 = 0;
				Column = 0;
			}
			public: ~FontClass()
			{
			}
			// 현재옵션
			private: const GraphClass* Graph;
			private: const int SavedHeight;
			private: color_x888 Color;
			private: byte Opacity;
			private: textalign Align;
			private: int Angle1024;
			private: int Column;
			// 출력요청
			private: typedef BxVarVector<DrawData, 128> DrawVector;
			private: BxVarMap<DrawVector> DrawListMap;
			private: BxVarVector<GLuint, 8> DrawIDList;
		};
		/// @endcond

		/// @cond SECTION_NAME
		class FontSingle
		{
			// Text
			public: TextClass* CreateText(string str, textencoding encoding)
			{
				TextClass* Result = BxNew_Param(TextClass, str, encoding);
				return Result;
			}
			public: TextClass* CreateText(wstring wstr, bool referonly)
			{
				TextClass* Result = BxNew_Param(TextClass, wstr, referonly);
				return Result;
			}
			public: void RemoveText(TextClass* text)
			{
				BxDelete(text);
			}
			// Graph
			public: bool CreateGraph(string nick, const byte* buffer, int buffersize)
			{
				return This->Graph(nick).Create(buffer, buffersize);
			}
			public: void RemoveGraph(string nick)
			{
				bool Result = This->Graph.Remove(nick);
				BxASSERT("BxCore::Font<존재하지 않는 nick을 삭제합니다>", Result);
			}
			public: bool IsExistGraph(string nick)
			{
				GraphClass* Graph = This->Graph.Access(nick);
				return (Graph && Graph->IsCreated());
			}
			// Font
			public: FontClass* CreateFont(string nick, fint pt)
			{
				FontClass* Result = BxNew_Param(FontClass, &This->Graph(nick), pt);
				return Result;
			}
			public: void RemoveFont(FontClass* font)
			{
				BxDelete(font);
			}
			// Etc
			void SetProgram(BxCore::Main::ProgramClass* program, BxCore::Main::ProgramClass* program_debug)
            {
                This->RefProgram = program;
				This->RefProgramDebug = program_debug;
            }
			QGLShaderProgram* GetFocusedProgram()
			{
				BxASSERT("BxCore::Font<Program이 nullptr입니다>", This->RefProgram);
				return This->RefProgram->SetFocus();
			}
			QGLShaderProgram* GetFocusedProgramDebug()
			{
				BxASSERT("BxCore::Font<Program(Debug)이 nullptr입니다>", This->RefProgramDebug);
				return This->RefProgramDebug->SetFocus();
			}
			// 멤버
			public: FontSingle() {global_data Data* _ = BxNew(Data); This = _;}
			public: ~FontSingle() {}
			private: class Data
            {
				public: Data() : RefProgram(nullptr), RefProgramDebug(nullptr) {}
				public: ~Data() {}
				public: BxVarMap<GraphClass> Graph;
				public: BxCore::Main::ProgramClass* RefProgram;
				public: BxCore::Main::ProgramClass* RefProgramDebug;
			} *This;
		};
		/// @endcond
    }

	namespace Thread
	{
		/// @cond SECTION_NAME
		class ThreadClass : public QThread
		{
			Q_OBJECT
		private:
			callback_thread CB;
			void* Data;
		protected:
			void run()
			{
				if(CB) CB(Data);
				BxSingleton::UnbindAll(true);
				BxPoolGlobal::UnbindAll();
				BxCore::Thread::UnbindStorageAll();
			}
		public:
			ThreadClass(callback_thread cb, void* data) : CB(cb), Data(data)
			{
				start();
			}
			virtual ~ThreadClass()
			{
			}
		};
		/// @endcond

		/// @cond SECTION_NAME
		class StorageClass
		{
		public:
			typedef BxVarMap<StorageClass, 0, true> VarMap;
			typedef BxVarMap<VarMap, 0, true> VarMapMap;
        private:
			void* Data;
            __DEBUG_VAL__
		public:
            StorageClass() : Data(nullptr) {}
            ~StorageClass() {BxCore::Util::Free(Data);}
            void* GetData() {return Data;}
            void* InitData(int size __DEBUG_PRM__)
            {
				Data = BxCore::Util::Alloc(size);
                BxCore::Util::MemSet(Data, 0, size);
                __DEBUG_SET__
                return Data;
            }
		public:
            global_func mint CurrentThreadID() {return (mint) QThread::currentThreadId();}
			global_func id_mutex& Mutex() {global_data id_mutex _ = OpenMutex(); return _;}
            global_func VarMapMap& Map() {global_data VarMapMap _; return _;}
		};
		/// @endcond
	}

	namespace OpenGL2D
	{
		/// @cond SECTION_NAME
		class ImageProgram : public BxCore::Main::ProgramClass
		{
		public:
            ImageProgram(const QGLContext* context) : BxCore::Main::ProgramClass(context) {}
			~ImageProgram() {}
		protected:
			string GetVertexScript()
			{
				return
					"uniform mat4 matrix;\n"
					"attribute vec4 vertex;\n"
					"attribute vec2 coord;\n"
					"uniform vec4 color;\n"
					"varying vec2 v_coord;\n"
					"varying vec4 v_color;\n"
					"void main(void)\n"
					"{\n"
					"    gl_Position = matrix * vertex;\n"
					"    v_coord = coord;\n"
					"    v_color = color;\n"
					"}\n";
			}
			string GetFragmentScript()
			{
				return
					"#ifdef GL_ES\n"
					"    precision mediump float;\n"
					"    precision mediump int;\n"
					"#endif\n"
					"uniform sampler2D texture;\n"
					"varying vec2 v_coord;\n"
					"varying vec4 v_color;\n"
					"void main(void)\n"
					"{\n"
					"    vec4 rgba = texture2D(texture, v_coord);\n"
					"    gl_FragColor = vec4(\n"
					"        rgba.x * v_color.x,\n"
					"        rgba.y * v_color.y,\n"
					"        rgba.z * v_color.z,\n"
					"        rgba.w * v_color.w);\n"
					"}\n";
			}
			void InitAttributes()
			{
				link();
				bindAttributeLocation("vertex", 0);
				bindAttributeLocation("coord", 1);
			}
			void InitUniforms()
			{
				QMatrix4x4 Matrix;
				int xRot = 0, yRot = 0, zRot = 0;
                Matrix.ortho(-0.5f / GLSCALE, +0.5f / GLSCALE, +0.5f / GLSCALE, -0.5f / GLSCALE, 4.0f, 15.0f);
                Matrix.scale(1.0f / GLSCALE);
				Matrix.translate(0.0f, 0.0f, -10.0f);
				Matrix.rotate(xRot / 16.0f, 1.0f, 0.0f, 0.0f);
				Matrix.rotate(yRot / 16.0f, 0.0f, 1.0f, 0.0f);
				Matrix.rotate(zRot / 16.0f, 0.0f, 0.0f, 1.0f);
				setUniformValue("matrix", Matrix);
				setUniformValue("texture", 0);
			}
			void Bind()
			{
				bind();
				enableAttributeArray(0);
				enableAttributeArray(1);
			}
			void Release()
			{
				disableAttributeArray(0);
				disableAttributeArray(1);
				release();
			}
		};
		class ColorProgram : public BxCore::Main::ProgramClass
		{
		public:
            ColorProgram(const QGLContext* context) : BxCore::Main::ProgramClass(context) {}
			~ColorProgram() {}
		protected:
			string GetVertexScript()
			{
				return
					"uniform mat4 matrix;\n"
					"attribute vec4 vertex;\n"
					"uniform vec4 color;\n"
					"varying vec4 v_color;\n"
					"void main(void)\n"
					"{\n"
					"    gl_Position = matrix * vertex;\n"
					"    v_color = color;\n"
					"}\n";
			}
			string GetFragmentScript()
			{
				return
					"#ifdef GL_ES\n"
					"    precision mediump float;\n"
					"    precision mediump int;\n"
					"#endif\n"
					"varying vec4 v_color;\n"
					"void main(void)\n"
					"{\n"
					"    gl_FragColor = v_color;\n"
					"}\n";
			}
			void InitAttributes()
			{
				link();
				bindAttributeLocation("vertex", 0);
			}
			void InitUniforms()
			{
				QMatrix4x4 Matrix;
				int xRot = 0, yRot = 0, zRot = 0;
                Matrix.ortho(-0.5f / GLSCALE, +0.5f / GLSCALE, +0.5f / GLSCALE, -0.5f / GLSCALE, 4.0f, 15.0f);
                Matrix.scale(1.0f / GLSCALE);
				Matrix.translate(0.0f, 0.0f, -10.0f);
				Matrix.rotate(xRot / 16.0f, 1.0f, 0.0f, 0.0f);
				Matrix.rotate(yRot / 16.0f, 0.0f, 1.0f, 0.0f);
				Matrix.rotate(zRot / 16.0f, 0.0f, 0.0f, 1.0f);
				setUniformValue("matrix", Matrix);
			}
			void Bind()
			{
				bind();
				enableAttributeArray(0);
			}
			void Release()
			{
				disableAttributeArray(0);
				release();
			}
		};
		class FontProgram : public BxCore::Main::ProgramClass
		{
		public:
            FontProgram(const QGLContext* context) : BxCore::Main::ProgramClass(context) {}
			~FontProgram() {}
		protected:
			string GetVertexScript()
			{
				return
					"uniform mat4 matrix;\n"
					"attribute vec4 vertex;\n"
					"attribute vec2 coord;\n"
					"uniform vec4 color;\n"
					"varying vec2 v_coord;\n"
					"varying vec4 v_color;\n"
					"void main(void)\n"
					"{\n"
					"    gl_Position = matrix * vertex;\n"
					"    v_coord = coord;\n"
					"    v_color = color;\n"
					"}\n";
			}
			string GetFragmentScript()
			{
				return
					"#ifdef GL_ES\n"
					"    precision mediump float;\n"
					"    precision mediump int;\n"
					"#endif\n"
					"uniform sampler2D texture;\n"
					"varying vec2 v_coord;\n"
					"varying vec4 v_color;\n"
					"void main(void)\n"
					"{\n"
					"    vec4 rgba = texture2D(texture, v_coord);\n"
					"    gl_FragColor = vec4(\n"
					"        v_color.x,\n"
					"        v_color.y,\n"
					"        v_color.z,\n"
					"        v_color.w * rgba.w);\n"
					"}\n";
			}
			void InitAttributes()
			{
				link();
				bindAttributeLocation("vertex", 0);
				bindAttributeLocation("coord", 1);
			}
			void InitUniforms()
			{
				QMatrix4x4 Matrix;
				int xRot = 0, yRot = 0, zRot = 0;
                Matrix.ortho(-0.5f / GLSCALE, +0.5f / GLSCALE, +0.5f / GLSCALE, -0.5f / GLSCALE, 4.0f, 15.0f);
                Matrix.scale(1.0f / GLSCALE);
				Matrix.translate(0.0f, 0.0f, -10.0f);
				Matrix.rotate(xRot / 16.0f, 1.0f, 0.0f, 0.0f);
				Matrix.rotate(yRot / 16.0f, 0.0f, 1.0f, 0.0f);
				Matrix.rotate(zRot / 16.0f, 0.0f, 0.0f, 1.0f);
				setUniformValue("matrix", Matrix);
				setUniformValue("texture", 0);
			}
			void Bind()
			{
				bind();
				enableAttributeArray(0);
				enableAttributeArray(1);
			}
			void Release()
			{
				disableAttributeArray(0);
				disableAttributeArray(1);
				release();
			}
		};
		/// @endcond

		/// @cond SECTION_NAME
        class OpenGLForm
        {
        public:
            enum OpenGLFormType {IMAGE, COLOR} Type;
            point UVRate;
            OpenGLForm(OpenGLFormType type) : Type(type)
            {
                UVRate.x = 1 << 11;
                UVRate.y = 1 << 11;
            }
            virtual ~OpenGLForm() {}
        };
        class TextureMaterial : public OpenGLForm
        {
        public:
			GLuint TextureID;
            TextureMaterial() : OpenGLForm(IMAGE)
            {
				TextureID = 0;
            }
			virtual ~TextureMaterial()
			{
				if(TextureID)
					glDeleteTextures(1, &TextureID);
			}
        };
        class ColorAmbient : public OpenGLForm
        {
        public:
            color_x888 Color;
            ColorAmbient() : OpenGLForm(COLOR) {}
            virtual ~ColorAmbient() {}
        };
		/// @endcond

		/// @cond SECTION_NAME
		class OpenGLOutline
		{
        public:
            enum OpenGLOutlineType {RECT, POLYGON, POLYGON3D, SPLINE} Type;
            OpenGLOutline(OpenGLOutlineType type) : Type(type) {}
            virtual ~OpenGLOutline() {}
		};
		class VertexUV : public OpenGLOutline
		{
        public:
            pointf Vertex[4];
            point UV[4];
            VertexUV() : OpenGLOutline(RECT) {}
            virtual ~VertexUV() {}
		};
		class PolyVertexUV : public OpenGLOutline
		{
        public:
            int Count;
            pointf* Vertex;
            point* UV;
            PolyVertexUV(int count) : OpenGLOutline(POLYGON)
            {
                Count = count;
                Vertex = BxNew_Array(pointf, count);
                UV = BxNew_Array(point, count);
            }
            virtual ~PolyVertexUV()
            {
                BxDelete_Array(Vertex);
                BxDelete_Array(UV);
            }
		};
		class PolyVertexUV3D : public OpenGLOutline
		{
        public:
            int Count;
            vertex* Vertex;
            point* UV;
            PolyVertexUV3D(int count) : OpenGLOutline(POLYGON3D)
            {
                Count = count;
                Vertex = BxNew_Array(vertex, count);
                UV = BxNew_Array(point, count);
            }
            virtual ~PolyVertexUV3D()
            {
                BxDelete_Array(Vertex);
                BxDelete_Array(UV);
            }
		};
		class SplineVertex : public OpenGLOutline
		{
        public:
            pointf VertexL[4];
			pointf VertexR[4];
			SplineVertex() : OpenGLOutline(SPLINE) {}
            virtual ~SplineVertex() {}
		};
		struct LinesData
		{
		public:
			float x;
			float y;
			float reserved;
		};
		struct StripData
		{
		public:
			float force;
			float lx;
			float ly;
			float rx;
			float ry;
		};
		/// @endcond

        /// @cond SECTION_NAME
        class OpenGLSingle
        {
        public:
            void BindForm(OpenGLForm* form)
            {
                switch(form->Type)
                {
                case OpenGLForm::IMAGE:
					This->LastProgramType = Data::IMG;
                    BxCore::Main::GLWidget::Me().BindTexture(((TextureMaterial*) form)->TextureID);
                    break;
                case OpenGLForm::COLOR:
					This->LastProgramType = Data::COL;
                    break;
                }
            }
            void DrawOutline(OpenGLOutline* outline, const point& uvrate, int x, int y, const byte opacity, const color_x888 color)
            {
				global_data int EdgeCount = 4;
				global_data QVector3D* Vertex = new QVector3D[EdgeCount];
				global_data QVector2D* UV = new QVector2D[EdgeCount];

                const int WidthHW = BxCore::Surface::SurfaceSingle().GetWidthHW();
                const int HeightHW = BxCore::Surface::SurfaceSingle().GetHeightHW();
                const float HalfWidth = WidthHW / 2.0f;
                const float HalfHeight = HeightHW / 2.0f;
                const float DeWidth = 1.0f / WidthHW;
                const float DeHeight = 1.0f / HeightHW;

                BxCore::Surface::SurfaceSingle().SetRenderMode(rendermode_2d);
				QGLShaderProgram* LastProgram = This->Program[This->LastProgramType]->SetFocus();
                switch(outline->Type)
                {
                case OpenGLOutline::RECT:
                    {
						VertexUV* VUV = (VertexUV*) outline;
						const int Count = 4;
                        // Vertex
                        for(int i = 0; i < Count; ++i)
                        {
                            Vertex[i].setX((FtoR(VUV->Vertex[i].x) + x - HalfWidth) * DeWidth);
                            Vertex[i].setY((FtoR(VUV->Vertex[i].y) + y - HalfHeight) * DeHeight);
                            Vertex[i].setZ(0);
                        }
						LastProgram->setAttributeArray(0, Vertex);
						// UV
						if(This->LastProgramType == Data::IMG)
						{
							for(int i = 0; i < Count; ++i)
							{
								const int X = (VUV->UV[i].x - (1 << 11)) * uvrate.x / BxIntegerX(1 << 11) + (1 << 11);
								const int Y = (VUV->UV[i].y - (1 << 11)) * uvrate.y / BxIntegerX(1 << 11) + (1 << 11);
								UV[i].setX(X * (1.0f / (1 << 12)));
								UV[i].setY(1.0f - Y * (1.0f / (1 << 12)));
							}
							LastProgram->setAttributeArray(1, UV);
						}
						// Color
						const int R = (color >> 16) & 0xFF;
						const int G = (color >> 8) & 0xFF;
						const int B = (color >> 0) & 0xFF;
						QColor Color(R, G, B, opacity);
						LastProgram->setUniformValue("color", Color);
						glDrawArrays(GL_TRIANGLE_FAN, 0, Count);
                    }
                    break;
                case OpenGLOutline::POLYGON:
                    {
						PolyVertexUV* PVUV = (PolyVertexUV*) outline;
						const int Count = PVUV->Count;
						if(EdgeCount < Count)
						{
							EdgeCount = Count;
							delete[] Vertex;
							delete[] UV;
							Vertex = new QVector3D[EdgeCount];
							UV = new QVector2D[EdgeCount];
						}
						// Vertex
                        for(int i = 0; i < Count; ++i)
                        {
                            Vertex[i].setX((FtoR(PVUV->Vertex[i].x) + x - HalfWidth) * DeWidth);
                            Vertex[i].setY((FtoR(PVUV->Vertex[i].y) + y - HalfHeight) * DeHeight);
                            Vertex[i].setZ(0);
                        }
						LastProgram->setAttributeArray(0, Vertex);
						// UV
						if(This->LastProgramType == Data::IMG)
						{
							for(int i = 0; i < Count; ++i)
							{
								const int X = (PVUV->UV[i].x - (1 << 11)) * uvrate.x / BxIntegerX(1 << 11) + (1 << 11);
								const int Y = (PVUV->UV[i].y - (1 << 11)) * uvrate.y / BxIntegerX(1 << 11) + (1 << 11);
								UV[i].setX(X * (1.0f / (1 << 12)));
								UV[i].setY(1.0f - Y * (1.0f / (1 << 12)));
							}
							LastProgram->setAttributeArray(1, UV);
						}
						// Color
						const int R = (color >> 16) & 0xFF;
						const int G = (color >> 8) & 0xFF;
						const int B = (color >> 0) & 0xFF;
						QColor Color(R, G, B, opacity);
						LastProgram->setUniformValue("color", Color);
						glDrawArrays(GL_TRIANGLE_FAN, 0, Count);
                    }
                    break;
                case OpenGLOutline::POLYGON3D:
                    //Program->setAttributeArray(0, ((PolyVertexUV3D*) outline)->Vertex);
                    //Program->setAttributeArray(0, ((PolyVertexUV3D*) outline)->UV);
                    //RefFunctions->glDrawArrays(GL_TRIANGLE_FAN, 0, ((PolyVertexUV3D*) outline)->Count);
                    break;
				case OpenGLOutline::SPLINE:
                    {
						SplineVertex* SV = (SplineVertex*) outline;
						pointf LB, LE;
						const fint LVecX = SV->VertexL[2].x - SV->VertexL[1].x;
						const fint LVecY = SV->VertexL[2].y - SV->VertexL[1].y;
						const fint LVecXB = SV->VertexL[1].x - SV->VertexL[0].x;
						const fint LVecYB = SV->VertexL[1].y - SV->VertexL[0].y;
						const fint LVecXE = SV->VertexL[3].x - SV->VertexL[2].x;
						const fint LVecYE = SV->VertexL[3].y - SV->VertexL[2].y;
						BxUtil::GetCubicSpline(LVecX, LVecY, LVecXB, LVecYB, LB, LVecXE, LVecYE, LE);
						pointf RB, RE;
						const fint RVecX = SV->VertexR[2].x - SV->VertexR[1].x;
						const fint RVecY = SV->VertexR[2].y - SV->VertexR[1].y;
						const fint RVecXB = SV->VertexR[1].x - SV->VertexR[0].x;
						const fint RVecYB = SV->VertexR[1].y - SV->VertexR[0].y;
						const fint RVecXE = SV->VertexR[3].x - SV->VertexR[2].x;
						const fint RVecYE = SV->VertexR[3].y - SV->VertexR[2].y;
						BxUtil::GetCubicSpline(RVecX, RVecY, RVecXB, RVecYB, RB, RVecXE, RVecYE, RE);

						const fint LMax = BxUtilGlobal::Max(BxUtilGlobal::Abs(LVecX), BxUtilGlobal::Abs(LVecY));
						const fint RMax = BxUtilGlobal::Max(BxUtilGlobal::Abs(RVecX), BxUtilGlobal::Abs(RVecY));
						const int CountSolo = BxUtilGlobal::Max(1, FtoI(BxUtilGlobal::Max(LMax, RMax)) / 2);
						const int Count = (CountSolo + 1) * 2;
						if(EdgeCount < Count)
						{
							EdgeCount = Count;
							delete[] Vertex;
							delete[] UV;
							Vertex = new QVector3D[EdgeCount];
							UV = new QVector2D[EdgeCount];
						}

						// Vertex
						for(int i = 0; i <= CountSolo; ++i)
						{
							const fint t = ItoF(i) / CountSolo;
							const fint tt = F_Multiply_F(t, t);
							const fint r = t - ItoF(1);
							const fint av = F_Multiply_F(tt, ItoF(3) - 2 * t);
							const fint bv = F_Multiply_F(t, F_Multiply_F(r, r));
							const fint cv = F_Multiply_F(tt, r);
							const fint lvx = SV->VertexL[1].x + F_Multiply_F(LVecX, av) + F_Multiply_F(LB.x, bv) + F_Multiply_F(LE.x, cv);
							const fint lvy = SV->VertexL[1].y + F_Multiply_F(LVecY, av) + F_Multiply_F(LB.y, bv) + F_Multiply_F(LE.y, cv);
							const fint rvx = SV->VertexR[1].x + F_Multiply_F(RVecX, av) + F_Multiply_F(RB.x, bv) + F_Multiply_F(RE.x, cv);
							const fint rvy = SV->VertexR[1].y + F_Multiply_F(RVecY, av) + F_Multiply_F(RB.y, bv) + F_Multiply_F(RE.y, cv);
							Vertex[i * 2 + 0].setX((FtoD(lvx) + x - HalfWidth) * DeWidth);
                            Vertex[i * 2 + 0].setY((FtoD(lvy) + y - HalfHeight) * DeHeight);
                            Vertex[i * 2 + 0].setZ(0);
							Vertex[i * 2 + 1].setX((FtoD(rvx) + x - HalfWidth) * DeWidth);
                            Vertex[i * 2 + 1].setY((FtoD(rvy) + y - HalfHeight) * DeHeight);
                            Vertex[i * 2 + 1].setZ(0);
						}
						LastProgram->setAttributeArray(0, Vertex);

						// Color
						const int R = (color >> 16) & 0xFF;
						const int G = (color >> 8) & 0xFF;
						const int B = (color >> 0) & 0xFF;
						QColor Color(R, G, B, opacity);
						LastProgram->setUniformValue("color", Color);

						#ifdef GL_QUAD_STRIP
							glDrawArrays(GL_QUAD_STRIP, 0, Count);
						#else
							glDrawArrays(GL_TRIANGLE_STRIP, 0, Count);
						#endif
                    }
                    break;
                }
            }
			void DrawLines(int Count, const LinesData* Data, const float x, const float y, const float scale, const byte opacity, const color_x888 color, const bool loop)
			{
                global_data int CountMax = 256;
                global_data QVector2D* Vertex = new QVector2D[CountMax];
                if(Count < 2) return;

                if(CountMax < Count)
                {
                    delete[] Vertex;
                    Vertex = new QVector2D[CountMax = Count];
                }

				BxCore::Surface::SurfaceSingle().SetRenderMode(rendermode_2d);
				QGLShaderProgram* LastProgram = This->Program[Data::COL]->SetFocus();
                const float DeWidth = scale / BxCore::Surface::GetWidthHW(false);
                const float DeHeight = scale / BxCore::Surface::GetHeightHW(false);
                const float DeX = x + BxCore::Main::GetGUIMarginL() / 2 / scale;
                const float DeY = y + BxCore::Main::GetGUIMarginT() / 2 / scale;
                const int R = (color >> 16) & 0xFF;
                const int G = (color >> 8) & 0xFF;
                const int B = (color >> 0) & 0xFF;
                const int A = opacity & 0xFF;

                // Vertex
                for(int i = 0; i < Count; ++i)
                {
                    Vertex[i].setX((Data[i].x + DeX) * DeWidth);
                    Vertex[i].setY((Data[i].y + DeY) * DeHeight);
                }
                LastProgram->setAttributeArray(0, Vertex);
                // Color
				QColor Color(R, G, B, A);
				LastProgram->setUniformValue("color", Color);
                // Draw
                glDrawArrays((loop)? GL_LINE_LOOP : GL_LINE_STRIP, 0, Count);
			}
            template<bool useStripOpacity, bool useMatrix>
            void DrawStrip(int Count, const StripData* Data, const byte opacity,
				const uint aqua, const color_x888 color, const float x, const float y, const float scale,
                const float m11, const float m12, const float m21, const float m22, const float dx, const float dy)
			{
				global_data const int CountMax = 256;
				global_data QVector2D Vertex[2 * CountMax];
				if(Count < 2) return;

				BxCore::Surface::SurfaceSingle().SetRenderMode(rendermode_2d);
				QGLShaderProgram* LastProgram = This->Program[Data::COL]->SetFocus();
				const float DeWidth = scale / BxCore::Surface::GetWidthHW(false);
                const float DeHeight = scale / BxCore::Surface::GetHeightHW(false);
                const float De11 = m11 * DeWidth;
                const float De12 = m12 * DeHeight;
                const float De21 = m21 * DeWidth;
                const float De22 = m22 * DeHeight;
                const float DeX = (dx + x + BxCore::Main::GetGUIMarginL() / 2 / scale) * DeWidth;
                const float DeY = (dy + y + BxCore::Main::GetGUIMarginT() / 2 / scale) * DeHeight;
				const int R = (color >> 16) & 0xFF;
				const int G = (color >> 8) & 0xFF;
				const int B = (color >> 0) & 0xFF;
				const int A = opacity & 0xFF;
				const float AquaForce = aqua / 255.0f;
				const float AquaValue = (0 < AquaForce)? A / AquaForce : 0;

				int FocusBegin = 1;
				while(FocusBegin < Count)
				{
					// Vertex
					int Focus = FocusBegin -= 1;
                    const int Opacity = (useStripOpacity && Data[Focus].force < AquaForce)?
						(int) (AquaValue * Data[Focus].force) : A;
					for(int i = 0; i < 2 * CountMax && Focus < Count; i += 2)
					{
						const StripData& CurData = Data[Focus];
						if(useMatrix)
						{
							Vertex[i + 0].setX(CurData.lx * De11 + CurData.ly * De21 + DeX);
							Vertex[i + 0].setY(CurData.lx * De12 + CurData.ly * De22 + DeY);
							Vertex[i + 1].setX(CurData.rx * De11 + CurData.ry * De21 + DeX);
							Vertex[i + 1].setY(CurData.rx * De12 + CurData.ry * De22 + DeY);
						}
						else
						{
							Vertex[i + 0].setX(CurData.lx * De11 + DeX);
							Vertex[i + 0].setY(CurData.ly * De22 + DeY);
							Vertex[i + 1].setX(CurData.rx * De11 + DeX);
							Vertex[i + 1].setY(CurData.ry * De22 + DeY);
						}
						Focus++;
						if(useStripOpacity)
						{
							const int CurOpacity = (Data[Focus - 1].force < AquaForce)?
								(int) (AquaValue * Data[Focus - 1].force) : A;
							if(Opacity != CurOpacity) break;
						}
					}
					LastProgram->setAttributeArray(0, Vertex);
                    // Color
                    QColor Color(R, G, B, Opacity);
					LastProgram->setUniformValue("color", Color);
                    // Draw
					#ifdef GL_QUAD_STRIP
						glDrawArrays(GL_QUAD_STRIP, 0, 2 * (Focus - FocusBegin));
					#else
						glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * (Focus - FocusBegin));
					#endif
					FocusBegin = Focus;
				}
			}
            void Flush()
            {
                glFlush();
            }
            void SwapBuffer()
            {
				BxCore::Main::GLWidget& Widget = BxCore::Main::GLWidget::Me();
				Widget.makeCurrent();
                Widget.swapBuffers();
            }
            void SetScissor(int x, int y, int w, int h)
            {
				global_data const int PixelRatio = BxCore::Main::GLWidget::Me().devicePixelRatio();
                y = BxCore::Surface::SurfaceSingle().GetHeightHW() - (y + h);
                glScissor(x * PixelRatio / GLSCALE, y * PixelRatio / GLSCALE,
					w * PixelRatio / GLSCALE, h * PixelRatio / GLSCALE);
            }
        public:
            OpenGLSingle() {global_data Data* Ref = BxNew(Data); This = Ref;}
        private:
            class Data
            {
            public:
				enum {IMG, COL, FNT, MAX} LastProgramType;
				BxCore::Main::ProgramClass* Program[MAX];
                Data()
                {
					BxCore::Main::GLWidget& Widget = BxCore::Main::GLWidget::Me();
					LastProgramType = COL;
                    Program[IMG] = BxNew_Param(ImageProgram, Widget.context());
					Program[IMG]->Initialize();
                    Program[COL] = BxNew_Param(ColorProgram, Widget.context());
					Program[COL]->Initialize();
                    Program[FNT] = BxNew_Param(FontProgram, Widget.context());
					Program[FNT]->Initialize();
					BxCore::Font::FontSingle().SetProgram(Program[FNT], Program[COL]);
                }
                ~Data()
                {
					for(int i = 0; i < MAX; ++i)
						BxDelete(Program[i]);
                }
            } *This;
        };
        /// @endcond
	}
}
