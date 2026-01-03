#pragma once
#include "MyForm.h"

namespace Ass {

	using namespace System;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;

	public ref class MyForm1 : public System::Windows::Forms::Form
	{
	public:
		MyForm1(void)
		{
			InitializeComponent();
		}

	protected:
		~MyForm1()
		{
			if (components) delete components;
		}

	private:
		System::Windows::Forms::Label^ labelTitle;
		System::Windows::Forms::Label^ labelDescription;
		System::Windows::Forms::Button^ btnStart;
		System::Windows::Forms::Button^ btnExit;
		System::Windows::Forms::Panel^ panelCard; // แผ่นหลังข้อความเพื่อให้ดูเป็นระเบียบ

		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->labelTitle = (gcnew System::Windows::Forms::Label());
			this->labelDescription = (gcnew System::Windows::Forms::Label());
			this->btnStart = (gcnew System::Windows::Forms::Button());
			this->btnExit = (gcnew System::Windows::Forms::Button());
			this->panelCard = (gcnew System::Windows::Forms::Panel());
			this->panelCard->SuspendLayout();
			this->SuspendLayout();
			// 
			// labelTitle
			// 
			this->labelTitle->Font = (gcnew System::Drawing::Font(L"Leelawadee UI", 28, System::Drawing::FontStyle::Bold));
			this->labelTitle->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(44)), static_cast<System::Int32>(static_cast<System::Byte>(62)),
				static_cast<System::Int32>(static_cast<System::Byte>(80)));
			this->labelTitle->Location = System::Drawing::Point(0, 60);
			this->labelTitle->Name = L"labelTitle";
			this->labelTitle->Size = System::Drawing::Size(484, 60);
			this->labelTitle->TabIndex = 0;
			this->labelTitle->Text = L"แปลภาษามือ";
			this->labelTitle->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// labelDescription
			// 
			this->labelDescription->Font = (gcnew System::Drawing::Font(L"Leelawadee UI", 11));
			this->labelDescription->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(127)),
				static_cast<System::Int32>(static_cast<System::Byte>(140)), static_cast<System::Int32>(static_cast<System::Byte>(141)));
			this->labelDescription->Location = System::Drawing::Point(40, 140);
			this->labelDescription->Name = L"labelDescription";
			this->labelDescription->Size = System::Drawing::Size(404, 100);
			this->labelDescription->TabIndex = 1;
			this->labelDescription->Text = L"สัมผัสประสบการณ์การสื่อสารรูปแบบใหม่\nใช้ AI ตรวจจับท่าทางของคุณผ่านกล้องแบบ Real-"
				L"time\nแม่นยำ ใช้งานง่าย และรวดเร็ว";
			this->labelDescription->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// btnStart
			// 
			this->btnStart->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(39)), static_cast<System::Int32>(static_cast<System::Byte>(174)),
				static_cast<System::Int32>(static_cast<System::Byte>(96)));
			this->btnStart->Cursor = System::Windows::Forms::Cursors::Hand;
			this->btnStart->FlatAppearance->BorderSize = 0;
			this->btnStart->FlatAppearance->MouseOverBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(46)),
				static_cast<System::Int32>(static_cast<System::Byte>(204)), static_cast<System::Int32>(static_cast<System::Byte>(113)));
			this->btnStart->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnStart->Font = (gcnew System::Drawing::Font(L"Leelawadee UI", 14, System::Drawing::FontStyle::Bold));
			this->btnStart->ForeColor = System::Drawing::Color::White;
			this->btnStart->Location = System::Drawing::Point(117, 280);
			this->btnStart->Name = L"btnStart";
			this->btnStart->Size = System::Drawing::Size(250, 60);
			this->btnStart->TabIndex = 2;
			this->btnStart->Text = L"START";
			this->btnStart->UseVisualStyleBackColor = false;
			this->btnStart->Click += gcnew System::EventHandler(this, &MyForm1::btnStart_Click);
			// 
			// btnExit
			// 
			this->btnExit->BackColor = System::Drawing::Color::Transparent;
			this->btnExit->Cursor = System::Windows::Forms::Cursors::Hand;
			this->btnExit->FlatAppearance->BorderColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(231)),
				static_cast<System::Int32>(static_cast<System::Byte>(76)), static_cast<System::Int32>(static_cast<System::Byte>(60)));
			this->btnExit->FlatAppearance->MouseOverBackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(231)),
				static_cast<System::Int32>(static_cast<System::Byte>(76)), static_cast<System::Int32>(static_cast<System::Byte>(60)));
			this->btnExit->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnExit->Font = (gcnew System::Drawing::Font(L"Leelawadee UI", 12));
			this->btnExit->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(231)), static_cast<System::Int32>(static_cast<System::Byte>(76)),
				static_cast<System::Int32>(static_cast<System::Byte>(60)));
			this->btnExit->Location = System::Drawing::Point(117, 360);
			this->btnExit->Name = L"btnExit";
			this->btnExit->Size = System::Drawing::Size(250, 50);
			this->btnExit->TabIndex = 3;
			this->btnExit->Text = L"EXIT";
			this->btnExit->UseVisualStyleBackColor = false;
			this->btnExit->Click += gcnew System::EventHandler(this, &MyForm1::btnExit_Click);
			this->btnExit->MouseEnter += gcnew System::EventHandler(this, &MyForm1::btnExit_MouseEnter);
			this->btnExit->MouseLeave += gcnew System::EventHandler(this, &MyForm1::btnExit_MouseLeave);
			// 
			// panelCard
			// 
			this->panelCard->BackColor = System::Drawing::Color::White;
			this->panelCard->Controls->Add(this->labelTitle);
			this->panelCard->Controls->Add(this->labelDescription);
			this->panelCard->Controls->Add(this->btnStart);
			this->panelCard->Controls->Add(this->btnExit);
			this->panelCard->Location = System::Drawing::Point(50, 40);
			this->panelCard->Name = L"panelCard";
			this->panelCard->Size = System::Drawing::Size(484, 480);
			this->panelCard->TabIndex = 0;
			// 
			// MyForm1
			// 
			this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(236)), static_cast<System::Int32>(static_cast<System::Byte>(240)),
				static_cast<System::Int32>(static_cast<System::Byte>(241)));
			this->ClientSize = System::Drawing::Size(584, 561);
			this->Controls->Add(this->panelCard);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->Name = L"MyForm1";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Sign Language App v1.0";
			this->panelCard->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion

		// --- Event สำหรับเปลี่ยนสีปุ่ม Exit เมื่อเอาเมาส์ชี้ (เพื่อให้ตัวหนังสือเปลี่ยนสีเป็นสีขาวด้วย) ---
		System::Void btnExit_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
			this->btnExit->ForeColor = System::Drawing::Color::White;
		}
		System::Void btnExit_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
			this->btnExit->ForeColor = System::Drawing::Color::FromArgb(231, 76, 60);
		}

		System::Void btnStart_Click(System::Object^ sender, System::EventArgs^ e) {
			MyForm^ cameraForm = gcnew MyForm();
			this->Hide();
			cameraForm->ShowDialog();
			this->Show();
		}

		System::Void btnExit_Click(System::Object^ sender, System::EventArgs^ e) {
			Application::Exit();
		}
	};
}