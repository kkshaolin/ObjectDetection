#pragma once

#include <vector> 
#include <opencv2/opencv.hpp> // ต้อง include เพื่อให้รู้จัก cv::Mat

namespace Ass {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();

			// --- แก้ไขจุดที่ 1: ย้ายการจองหน่วยความจำมาไว้ที่นี่ และลบการประกาศฟังก์ชันซ้อนออก ---
			fingerHistory = new std::vector<int>();

			isRunning = false;
			isDrawingROI = false;
			hasROI = false;
			tempBitmap = nullptr;
		}

	protected:
		~MyForm()
		{
			if (components) delete components;
			// คืนหน่วยความจำของ Native Vector
			if (fingerHistory) {
				delete fingerHistory;
				fingerHistory = nullptr;
			}
		}

	private:
		// ตัวแปร Native C++
		std::vector<int>* fingerHistory;

		// UI Components
		System::Windows::Forms::SplitContainer^ splitContainer1;
		System::Windows::Forms::Button^ startbutton;
		System::Windows::Forms::Button^ stopbutton;
		System::Windows::Forms::Button^ roiButton;
		System::Windows::Forms::PictureBox^ pictureBox1;

		// ตัวแปรควบคุมสถานะ
		bool isRunning;
		bool isDrawingROI;
		bool hasROI;
		System::Drawing::Point roiStartPoint;
		System::Drawing::Point roiEndPoint;
		System::Drawing::Bitmap^ tempBitmap;
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->roiButton = (gcnew System::Windows::Forms::Button());
			this->stopbutton = (gcnew System::Windows::Forms::Button());
			this->startbutton = (gcnew System::Windows::Forms::Button());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer1))->BeginInit();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// splitContainer1
			// 
			this->splitContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer1->Location = System::Drawing::Point(0, 0);
			this->splitContainer1->Name = L"splitContainer1";
			// 
			// splitContainer1.Panel1
			// 
			this->splitContainer1->Panel1->Controls->Add(this->roiButton);
			this->splitContainer1->Panel1->Controls->Add(this->stopbutton);
			this->splitContainer1->Panel1->Controls->Add(this->startbutton);
			// 
			// splitContainer1.Panel2
			// 
			this->splitContainer1->Panel2->Controls->Add(this->pictureBox1);
			this->splitContainer1->Size = System::Drawing::Size(949, 443);
			this->splitContainer1->SplitterDistance = 189;
			this->splitContainer1->TabIndex = 0;
			// 
			// roiButton
			// 
			this->roiButton->Location = System::Drawing::Point(63, 258);
			this->roiButton->Name = L"roiButton";
			this->roiButton->Size = System::Drawing::Size(80, 154);
			this->roiButton->TabIndex = 2;
			this->roiButton->Text = L"Set ROI";
			this->roiButton->UseVisualStyleBackColor = true;
			this->roiButton->Click += gcnew System::EventHandler(this, &MyForm::roiButton_Click);
			// 
			// stopbutton
			// 
			this->stopbutton->Enabled = false;
			this->stopbutton->Location = System::Drawing::Point(63, 174);
			this->stopbutton->Name = L"stopbutton";
			this->stopbutton->Size = System::Drawing::Size(80, 78);
			this->stopbutton->TabIndex = 1;
			this->stopbutton->Text = L"Stop";
			this->stopbutton->UseVisualStyleBackColor = true;
			this->stopbutton->Click += gcnew System::EventHandler(this, &MyForm::stopbutton_Click);
			// 
			// startbutton
			// 
			this->startbutton->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->startbutton->ForeColor = System::Drawing::SystemColors::ActiveCaption;
			this->startbutton->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"startbutton.Image")));
			this->startbutton->Location = System::Drawing::Point(35, 23);
			this->startbutton->Name = L"startbutton";
			this->startbutton->Size = System::Drawing::Size(134, 122);
			this->startbutton->TabIndex = 0;
			this->startbutton->Text = L"Start";
			this->startbutton->UseVisualStyleBackColor = true;
			this->startbutton->Click += gcnew System::EventHandler(this, &MyForm::startbutton_Click);
			// 
			// pictureBox1
			// 
			this->pictureBox1->BackColor = System::Drawing::Color::Black;
			this->pictureBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pictureBox1->Location = System::Drawing::Point(0, 0);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(756, 443);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			this->pictureBox1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::pictureBox1_MouseDown);
			this->pictureBox1->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::pictureBox1_MouseMove);
			this->pictureBox1->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::pictureBox1_MouseUp);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(949, 443);
			this->Controls->Add(this->splitContainer1);
			this->Name = L"MyForm";
			this->Text = L"Camera Application";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MyForm::MyForm_FormClosing);
			this->splitContainer1->Panel1->ResumeLayout(false);
			this->splitContainer1->Panel2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer1))->EndInit();
			this->splitContainer1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

		// --- แก้ไขจุดที่ 2: ประกาศฟังก์ชันให้เป็นสมาชิกของคลาสที่นี่ ---
		int CountFingers(const cv::Mat& roi);

		System::Void startbutton_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void stopbutton_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void roiButton_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void pictureBox1_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
		System::Void pictureBox1_MouseMove(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
		System::Void pictureBox1_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
		System::Void MyForm_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e);

		System::Drawing::Rectangle GetImageDisplayRectangle();
		System::Drawing::Point ControlToImagePoint(System::Drawing::Point p);
	};
}