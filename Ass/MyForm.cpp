#include "MyForm.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

using namespace cv;
using namespace System::Drawing;
namespace Ass {

    // ฟังก์ชันสำหรับแปลงจำนวนนิ้วเป็นความหมายภาษามือ
        std::string MyForm::TranslateSign(int fingerCount) {
            switch (fingerCount) {
            case 5:
                return "HELLO";    // ชู 5 นิ้ว = สวัสดี
            case 2:
                return "VICTORY";  // ชู 2 นิ้ว = สู้ๆ/สันติภาพ
            case 1:
                return "POINT";    // ชู 1 นิ้ว = ชี้
            case 0:
                return "FIST";     // กำหมัด
            default:
                return "WAITING...";
            }
        }
     
        int MyForm::CountFingers(const cv::Mat& roi) { // ฟังก์ชันสำหรับนับจำนวนนิ้วมือ

        static std::vector<int> fingerHistory; //ประวัติการนับ
        const int MAX_HISTORY = 17; // frame ที่ใช้คำนวณ

        if (roi.empty()) return 0; //ภาพเปล่า

        cv::Mat hsv, mask, maskColor; //เก็บภาพ
        cv::cvtColor(roi, hsv, cv::COLOR_BGR2HSV); //ข้อจำกัด ต้องใช้ในที่แสงคงที่

        cv::inRange(hsv, cv::Scalar(0, 30, 60), cv::Scalar(20, 150, 255), mask);

        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)); //สร้างรูปทรงวงรี
        
        // filter
        //cv::erode(mask, mask, kernel);
        //cv::dilate(mask, mask, kernel);
        cv::GaussianBlur(mask, mask, cv::Size(5, 5), 0);
        cv::threshold(mask, mask, 127, 255, cv::THRESH_BINARY);


        std::vector<std::vector<cv::Point>> contours; //พิกัดเส้นรอบรูป
		cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);//หาเส้นรอบรูป

		if (contours.empty()) return 0; //ถ้าไม่พบเส้นรอบรูป

		int largestIdx = -1; //เก็บลำดับเส้นรอบรูปที่ใหญ่ที่สุด
		double maxArea = 0; //เก็บพื้นที่เส้นรอบรูปที่ใหญ่ที่สุด
        
        // วาดเส้นรอบรูป 
        cv::drawContours(roi, contours, largestIdx, cv::Scalar(255, 0, 0), 2);

		for (size_t i = 0; i < contours.size(); i++) { //วนลูปหาเส้นรอบรูปที่ใหญ่ที่สุด
            double area = cv::contourArea(contours[i]);
            if (area > maxArea) {
                maxArea = area;
                largestIdx = (int)i;
            }
        }

        if (largestIdx == -1 || maxArea < 3000) return 0; //ไม่พบ เล็กเกิน

		std::vector<int> hullIndices; //เก็บจุดยอด convex hull(เส้นคลุมมื่อ)
		cv::convexHull(contours[largestIdx], hullIndices, false); //หาเส้น convex hull


        int validDefects = 0; //จำนวนร่องนิ้ว
        if (hullIndices.size() > 3) {
			std::vector<cv::Vec4i> defects; //เก็บข้อมูลร่องนิ้ว
            try {
				cv::convexityDefects(contours[largestIdx], hullIndices, defects); //หา ร่องนิ้ว
				for (const auto& defect : defects) { //วนลูปร่องนิ้ว
					float depth = defect[3] / 256.0f; //หาความลึกร่องนิ้ว

					cv::Point pStart = contours[largestIdx][defect[0]]; //จุดเริ่มต้นร่องนิ้ว
					cv::Point pEnd = contours[largestIdx][defect[1]]; //จุดสิ้นสุดร่องนิ้ว
					cv::Point pFar = contours[largestIdx][defect[2]]; //จุดลึกสุดร่องนิ้ว

                    // ก่อนวาด defect ให้แปลง mask เป็น BGR
                    cv::cvtColor(mask, maskColor, cv::COLOR_GRAY2BGR);
                    // วาด defect ด้วยสี
                    cv::line(maskColor, pStart, pFar, cv::Scalar(0, 255, 255), 2); // Yellow
                    cv::line(maskColor, pEnd, pFar, cv::Scalar(0, 0, 255), 2);     // Red
                    cv::circle(maskColor, pFar, 6, cv::Scalar(0, 255, 0), -1);     // Green

                    if (depth > 40.0f) { // หาองศาร่องนิ้ว
                        double a = std::sqrt(std::pow(pEnd.x - pStart.x, 2) + std::pow(pEnd.y - pStart.y, 2));
                        double b = std::sqrt(std::pow(pFar.x - pStart.x, 2) + std::pow(pFar.y - pStart.y, 2));
                        double c = std::sqrt(std::pow(pFar.x - pEnd.x, 2) + std::pow(pFar.y - pEnd.y, 2));
                        double angle = std::acos((b * b + c * c - a * a) / (2 * b * c)) * 57.2958;
						

						if (angle < 90) { // ถ้าองศาน้อยกว่า 90 องศานับเป็น 1 ร่องนิ้ว
                            validDefects++;
                        }
                    }
                }
            }
            catch (...) { return 0; } // ผิดพลาดคืน 0
        }

		int rawResult = (validDefects == 0) ? 1 : (validDefects + 1); //นิ้ว = ร่องนิ้ว + 1
		if (rawResult > 5) rawResult = 5; //จำกัดสูงสุด 5 นิ้ว

		fingerHistory.push_back(rawResult); //เก็บประวัติ
        if (fingerHistory.size() > MAX_HISTORY) {
            fingerHistory.erase(fingerHistory.begin());
        }

		std::map<int, int> counts; //นับความถี่ของนิ้วที่นับได้
        for (int v : fingerHistory) {
            counts[v]++;
        }

		int stableResult = rawResult; //ผลลัพธ์นิ้วที่เสถียร
		int maxFreq = 0; //ความถี่สูงสุด
		for (auto it = counts.begin(); it != counts.end(); ++it) { //หา นิ้วที่นับได้บ่อยที่สุด
            if (it->second > maxFreq) {
                maxFreq = it->second;
                stableResult = it->first;
            }
        }
        cv::imshow("Debug: Processed Mask", maskColor);
        cv::waitKey(1);

		return stableResult; //จำนวน นิ้วมือที่นับได้
    }

    System::Drawing::Rectangle MyForm::GetImageDisplayRectangle() { // หาขอบPictureBox
        if (this->pictureBox1->Image == nullptr)
            return System::Drawing::Rectangle(0, 0, this->pictureBox1->ClientSize.Width, this->pictureBox1->ClientSize.Height);
        int imgW = this->pictureBox1->Image->Width;
        int imgH = this->pictureBox1->Image->Height;
        int ctlW = this->pictureBox1->ClientSize.Width;
        int ctlH = this->pictureBox1->ClientSize.Height;
        float ratio = Math::Min((float)ctlW / imgW, (float)ctlH / imgH);
        int dispW = (int)(imgW * ratio);
        int dispH = (int)(imgH * ratio);
        return System::Drawing::Rectangle((ctlW - dispW) / 2, (ctlH - dispH) / 2, dispW, dispH);
    }

    System::Drawing::Point MyForm::ControlToImagePoint(System::Drawing::Point p) { // พิกัดเมาส์toพิกัดรูป
        if (this->pictureBox1->Image == nullptr) return p;
        System::Drawing::Rectangle r = GetImageDisplayRectangle();
        int x = Math::Max(r.X, Math::Min(p.X, r.X + r.Width - 1));
        int y = Math::Max(r.Y, Math::Min(p.Y, r.Y + r.Height - 1));
        int ix = (int)((x - r.X) * ((float)this->pictureBox1->Image->Width / r.Width));
        int iy = (int)((y - r.Y) * ((float)this->pictureBox1->Image->Height / r.Height));
        return System::Drawing::Point(ix, iy);
    }

    System::Void MyForm::startbutton_Click(System::Object^ sender, System::EventArgs^ e) {
        if (isRunning) { isRunning = false; return; } // หากกล้องทำงานอยู่

        try {
            VideoCapture capture(1); //เปิดกล้อง
            if (!capture.isOpened()) {
                MessageBox::Show("Image Error");
                return;
            }

            int w = (int)capture.get(CAP_PROP_FRAME_WIDTH); //กว้าง 
            int h = (int)capture.get(CAP_PROP_FRAME_HEIGHT);//สูง
            
            // สร้าง Bitmap ภาพ
            System::Drawing::Bitmap^ bmp = gcnew System::Drawing::Bitmap(w, h, System::Drawing::Imaging::PixelFormat::Format24bppRgb);
            System::Drawing::Rectangle rect(0, 0, w, h); //ขอบภาพ

            isRunning = true; //ภ้ากล้องเปิด เปลียนปุ่ม
            this->startbutton->Text = L"Stop";
            this->stopbutton->Enabled = true;
            this->roiButton->Enabled = false;

            while (isRunning) {
                // สร้างหน่วยความจำ สร้างรูปชนิด Bat ใช้กับ OpenCV
                auto bmpData = bmp->LockBits(rect, System::Drawing::Imaging::ImageLockMode::WriteOnly, bmp->PixelFormat);
                Mat frame(h, w, CV_8UC3, bmpData->Scan0.ToPointer(), bmpData->Stride);

                if (!capture.read(frame)) { // ถ้าอ่านภาพไม่ได้ สั่งหยุด
                    isRunning = false;
                    bmp->UnlockBits(bmpData);
                    break;
                }

                flip(frame, frame, 1); //พลิกภาพ

                if (hasROI) { //มีพื้นที่ ROI
                    int x1 = Math::Max(0, Math::Min(roiStartPoint.X, roiEndPoint.X));
                    int y1 = Math::Max(0, Math::Min(roiStartPoint.Y, roiEndPoint.Y));
                    int x2 = Math::Min(frame.cols - 1, Math::Max(roiStartPoint.X, roiEndPoint.X));
                    int y2 = Math::Min(frame.rows - 1, Math::Max(roiStartPoint.Y, roiEndPoint.Y));

                    if (x2 > x1 && y2 > y1) {
                        Mat overlay = frame.clone();
                        overlay.setTo(Scalar(0, 0, 0));
                        addWeighted(frame, 0.5, overlay, 0.5, 0, frame);

                        Mat roiRect = frame(cv::Rect(x1, y1, x2 - x1, y2 - y1));
                        cv::Mat roiProcess = roiRect.clone();

                        int result = CountFingers(roiProcess);
                        
                        std::string signMsg = TranslateSign(result);


                        rectangle(frame, cv::Point(x1, y1), cv::Point(x2, y2), Scalar(0, 255, 0), 3);
                        cv::putText(frame, "Fingers: " + std::to_string(result),
                            cv::Point(x1, y1 - 10), cv::FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 255, 0), 2);

                        cv::putText(frame, "Sign: " + signMsg,
                            cv::Point(x1, y1 - 50), cv::FONT_HERSHEY_DUPLEX, 1.2, Scalar(0, 255, 255), 2);
                    }
                }

                bmp->UnlockBits(bmpData);

                // แสดงผลภาพ
                this->pictureBox1->Image = bmp;

                Application::DoEvents();
            }

            // ปิดหน้าต่างและ reset button
            capture.release();
            cv::destroyAllWindows();
            this->startbutton->Text = L"Start";
            this->stopbutton->Enabled = false;
            this->roiButton->Enabled = true;
        }
        catch (std::exception& ex) { MessageBox::Show(gcnew System::String(ex.what()), "Error"); isRunning = false; }
    }

    System::Void MyForm::stopbutton_Click(System::Object^ sender, System::EventArgs^ e) { 
        isRunning = false; 
    }

    System::Void MyForm::roiButton_Click(System::Object^ sender, System::EventArgs^ e) {
        if (hasROI) {
            hasROI = false;
            this->roiButton->Text = L"Set ROI";
            this->Text = L"Camera Application";
        }
        else {
            if (isRunning) { MessageBox::Show("¡ÃØ³Ò Stop ¡ÅéÍ§¡èÍ¹ÇÒ´ ROI", "¤Óá¹Ð¹Ó"); return; }
            this->roiButton->Text = L"Drawing...";
            this->Text = L"Click and drag to draw ROI";
        }
    }

    System::Void MyForm::pictureBox1_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
        if (this->roiButton->Text == L"Drawing..." && !isRunning && this->pictureBox1->Image != nullptr) {
            isDrawingROI = true;
            this->roiStartPoint = ControlToImagePoint(System::Drawing::Point(e->X, e->Y));
            this->roiEndPoint = this->roiStartPoint;
            this->pictureBox1->Cursor = Cursors::Cross;
            if (this->tempBitmap != nullptr) delete this->tempBitmap;
            this->tempBitmap = (System::Drawing::Bitmap^)this->pictureBox1->Image->Clone();
        }
    }

    System::Void MyForm::pictureBox1_MouseMove(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
        if (isDrawingROI && this->tempBitmap != nullptr) {
            this->roiEndPoint = ControlToImagePoint(System::Drawing::Point(e->X, e->Y));
            System::Drawing::Bitmap^ drawBitmap = (System::Drawing::Bitmap^)this->tempBitmap->Clone();
            Graphics^ g = Graphics::FromImage(drawBitmap);
            Pen^ pen = gcnew Pen(Color::Lime, 2);
            int x = Math::Min(this->roiStartPoint.X, this->roiEndPoint.X);
            int y = Math::Min(this->roiStartPoint.Y, this->roiEndPoint.Y);
            int width = Math::Abs(this->roiEndPoint.X - this->roiStartPoint.X);
            int height = Math::Abs(this->roiEndPoint.Y - this->roiStartPoint.Y);
            g->DrawRectangle(pen, x, y, width, height);
            delete g; delete pen;
            if (this->pictureBox1->Image != this->tempBitmap) delete this->pictureBox1->Image;
            this->pictureBox1->Image = drawBitmap;
        }
    }

    System::Void MyForm::pictureBox1_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
        if (isDrawingROI) {
            this->roiEndPoint = ControlToImagePoint(System::Drawing::Point(e->X, e->Y));
            isDrawingROI = false;
            int width = Math::Abs(this->roiEndPoint.X - this->roiStartPoint.X);
            int height = Math::Abs(this->roiEndPoint.Y - this->roiStartPoint.Y);
            if (width > 10 && height > 10) {
                hasROI = true;
                this->roiButton->Text = L"Clear ROI";
                this->pictureBox1->Cursor = Cursors::Default;
            }
            else {
                hasROI = false;
                this->roiButton->Text = L"Set ROI";
                if (this->tempBitmap != nullptr) this->pictureBox1->Image = this->tempBitmap;
            }
        }
    }

    System::Void MyForm::MyForm_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e) { isRunning = false; }
} // namespace Ass 