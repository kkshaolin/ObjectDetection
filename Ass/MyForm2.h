#pragma once

namespace Ass {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class MyForm2 : public System::Windows::Forms::Form
	{
	public:
		// รับค่ารูปภาพและข้อความจากหน้ากล้องมาแสดงผล
		MyForm2(System::Drawing::Image^ capturedImg, System::String^ gestureText)
		{
			InitializeComponent();
			this->pictureBoxResult->Image = capturedImg;
			this->labelMeaning->Text = gestureText;
		}

	protected:
		~MyForm2()
		{
			if (components) delete components;
		}

	private:
		System::Windows::Forms::SplitContainer^ splitContainer1;
		System::Windows::Forms::PictureBox^ pictureBoxResult;
		System::Windows::Forms::Label^ labelHeader;
		System::Windows::Forms::Label^ labelMeaning;
		System::Windows::Forms::Button^ btnClose;

		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->pictureBoxResult = (gcnew System::Windows::Forms::PictureBox());
			this->labelHeader = (gcnew System::Windows::Forms::Label());
			this->labelMeaning = (gcnew System::Windows::Forms::Label());
			this->btnClose = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer1))->BeginInit();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxResult))->BeginInit();
			this->SuspendLayout();

			// 
			// splitContainer1 (แบ่งหน้าจอซ้าย-ขวา)
			// 
			this->splitContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer1->Location = System::Drawing::Point(0, 0);
			this->splitContainer1->Name = L"splitContainer1";
			// Panel 1 (ฝั่งซ้าย - PictureBox)
			this->splitContainer1->Panel1->BackColor = System::Drawing::Color::FromArgb(30, 39, 46);
			this->splitContainer1->Panel1->Controls->Add(this->pictureBoxResult);
			this->splitContainer1->Panel1->Padding = System::Windows::Forms::Padding(20);
			// Panel 2 (ฝั่งขวา - Text)
			this->splitContainer1->Panel2->BackColor = System::Drawing::Color::White;
			this->splitContainer1->Panel2->Controls->Add(this->btnClose);
			this->splitContainer1->Panel2->Controls->Add(this->labelMeaning);
			this->splitContainer1->Panel2->Controls->Add(this->labelHeader);
			this->splitContainer1->Size = System::Drawing::Size(800, 450);
			this->splitContainer1->SplitterDistance = 450; // กำหนดความกว้างฝั่งซ้าย
			this->splitContainer1->TabIndex = 0;

			// 
			// pictureBoxResult (แสดงภาพท่าทาง)
			// 
			this->pictureBoxResult->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pictureBoxResult->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBoxResult->Name = L"pictureBoxResult";
			this->pictureBoxResult->BackColor = System::Drawing::Color::FromArgb(45, 52, 54);

			// 
			// labelHeader (หัวข้อฝั่งขวา)
			// 
			this->labelHeader->Font = (gcnew System::Drawing::Font(L"Leelawadee UI", 14, System::Drawing::FontStyle::Bold));
			this->labelHeader->ForeColor = System::Drawing::Color::FromArgb(127, 140, 141);
			this->labelHeader->Location = System::Drawing::Point(20, 40);
			this->labelHeader->Size = System::Drawing::Size(300, 30);
			this->labelHeader->Text = L"ความหมายที่ตรวจพบ:";

			// 
			// labelMeaning (ข้อความแสดงความหมาย)
			// 
			this->labelMeaning->Font = (gcnew System::Drawing::Font(L"Leelawadee UI", 36, System::Drawing::FontStyle::Bold));
			this->labelMeaning->ForeColor = System::Drawing::Color::FromArgb(46, 204, 113);
			this->labelMeaning->Location = System::Drawing::Point(20, 80);
			this->labelMeaning->Size = System::Drawing::Size(300, 250);
			this->labelMeaning->Text = L"รอกล้อง...";
			this->labelMeaning->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;

			// 
			// btnClose
			// 
			this->btnClose->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->btnClose->BackColor = System::Drawing::Color::FromArgb(45, 52, 54);
			this->btnClose->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btnClose->Font = (gcnew System::Drawing::Font(L"Leelawadee UI", 10, System::Drawing::FontStyle::Bold));
			this->btnClose->ForeColor = System::Drawing::Color::White;
			this->btnClose->Location = System::Drawing::Point(180, 380);
			this->btnClose->Size = System::Drawing::Size(130, 45);
			this->btnClose->Text = L"กลับไปหน้ากล้อง";
			this->btnClose->UseVisualStyleBackColor = false;
			this->btnClose->Click += gcnew System::EventHandler(this, &MyForm2::btnClose_Click);

			// 
			// MyForm2
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(800, 450);
			this->Controls->Add(this->splitContainer1);
			this->Name = L"MyForm2";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Sign Language Interpretation";
			this->splitContainer1->Panel1->ResumeLayout(false);
			this->splitContainer1->Panel2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer1))->EndInit();
			this->splitContainer1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxResult))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

	private:
		System::Void btnClose_Click(System::Object^ sender, System::EventArgs^ e) {
			this->Close(); // ปิดหน้าความหมายเพื่อกลับไปหน้ากล้อง
		}
	};
}