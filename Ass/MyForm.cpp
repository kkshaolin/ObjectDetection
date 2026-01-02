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
     
        // --- ตัวแปรสำหรับเก็บประวัติค่าที่นับได้ (เพื่อลดการแกว่ง) ---


        // ฟังก์ชันสำหรับนับนิ้ว (ปรับปรุงใหม่)
        int MyForm::CountFingers(const cv::Mat& roi) {

        static std::vector<int> fingerHistory;
        const int MAX_HISTORY = 17;

        if (roi.empty()) return 0;

        cv::Mat ycrcb, mask;
        // 1. แปลงเป็น YCrCb เพื่อสู้กับแสงย้อนและการเปลี่ยนความสว่าง
        cv::cvtColor(roi, ycrcb, cv::COLOR_BGR2YCrCb);

        // ช่วงสีผิวมาตรฐานในระบบ YCrCb
        cv::inRange(ycrcb, cv::Scalar(0, 133, 77), cv::Scalar(255, 173, 127), mask);

        // 2. ถมรูโหว่กลางฝ่ามือด้วย Morphology Close และลด Noise
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
        cv::erode(mask, mask, kernel);
        cv::dilate(mask, mask, kernel);
        cv::GaussianBlur(mask, mask, cv::Size(5, 5), 0);
        cv::threshold(mask, mask, 127, 255, cv::THRESH_BINARY);

        // แสดงผล Mask เพื่อ Debug (สามารถปิดได้ทีหลัง)
        cv::imshow("Debug: Processed Mask", mask);

        // 3. หา Contour ที่ใหญ่ที่สุด
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        if (contours.empty()) return 0;

        int largestIdx = -1;
        double maxArea = 0;
        for (size_t i = 0; i < contours.size(); i++) {
            double area = cv::contourArea(contours[i]);
            if (area > maxArea) {
                maxArea = area;
                largestIdx = (int)i;
            }
        }

        // ปรับพื้นที่ขั้นต่ำเป็น 2000 เพื่อตัด Noise
        if (largestIdx == -1 || maxArea < 3000) return 0;

        // 4. หา Convex Hull และ Defects (ร่องนิ้ว)
        std::vector<int> hullIndices;
        // สำคัญ: ต้องเป็น false เพื่อให้ได้ดัชนีไปใช้หา defects
        cv::convexHull(contours[largestIdx], hullIndices, false);

        int validDefects = 0;
        if (hullIndices.size() > 3) {
            std::vector<cv::Vec4i> defects;
            try {
                cv::convexityDefects(contours[largestIdx], hullIndices, defects);
                for (const auto& defect : defects) {
                    float depth = defect[3] / 256.0f;

                    // จุดเริ่ม, จุดจบ, และจุดร่อง (Far point)
                    cv::Point pStart = contours[largestIdx][defect[0]];
                    cv::Point pEnd = contours[largestIdx][defect[1]];
                    cv::Point pFar = contours[largestIdx][defect[2]];

                    // ฟิลเตอร์ที่ 1: ความลึกร่องนิ้วต้องมากพอ
                    if (depth > 40.0f) {
                        // ฟิลเตอร์ที่ 2: คำนวณมุมที่ร่องนิ้ว (Cosine Rule)
                        double a = std::sqrt(std::pow(pEnd.x - pStart.x, 2) + std::pow(pEnd.y - pStart.y, 2));
                        double b = std::sqrt(std::pow(pFar.x - pStart.x, 2) + std::pow(pFar.y - pStart.y, 2));
                        double c = std::sqrt(std::pow(pFar.x - pEnd.x, 2) + std::pow(pFar.y - pEnd.y, 2));
                        double angle = std::acos((b * b + c * c - a * a) / (2 * b * c)) * 57.2958;

                        // ถ้าน้อยกว่า 90 องศา คือร่องนิ้วที่แท้จริง
                        if (angle < 90) {
                            validDefects++;
                        }
                    }
                }
            }
            catch (...) { return 0; }
        }

        // จำนวนนิ้ว (ร่อง + 1)
        int rawResult = (validDefects == 0) ? 1 : (validDefects + 1);
        if (rawResult > 5) rawResult = 5;

        // --- 5. ระบบ Filter ป้องกันค่าแกว่ง (Mode Filter) ---
        fingerHistory.push_back(rawResult); // แก้ไขจาก -> เป็น .
        if (fingerHistory.size() > MAX_HISTORY) {
            fingerHistory.erase(fingerHistory.begin());
        }

        std::map<int, int> counts;
        for (int v : fingerHistory) {
            counts[v]++;
        }

        int stableResult = rawResult;
        int maxFreq = 0;
        for (auto it = counts.begin(); it != counts.end(); ++it) {
            if (it->second > maxFreq) {
                maxFreq = it->second;
                stableResult = it->first;
            }
        }

        return stableResult;
    }

    // --- Helper Functions ---
    System::Drawing::Rectangle MyForm::GetImageDisplayRectangle() {
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

    System::Drawing::Point MyForm::ControlToImagePoint(System::Drawing::Point p) {
        if (this->pictureBox1->Image == nullptr) return p;
        System::Drawing::Rectangle r = GetImageDisplayRectangle();
        int x = Math::Max(r.X, Math::Min(p.X, r.X + r.Width - 1));
        int y = Math::Max(r.Y, Math::Min(p.Y, r.Y + r.Height - 1));
        int ix = (int)((x - r.X) * ((float)this->pictureBox1->Image->Width / r.Width));
        int iy = (int)((y - r.Y) * ((float)this->pictureBox1->Image->Height / r.Height));
        return System::Drawing::Point(ix, iy);
    }

    // --- ส่วนหลัก: startbutton_Click ---
    System::Void MyForm::startbutton_Click(System::Object^ sender, System::EventArgs^ e) {
        if (isRunning) { isRunning = false; return; }

        try {
            VideoCapture capture(0);
            if (!capture.isOpened()) {
                MessageBox::Show("ไม่สามารถเปิดกล้องได้", "Error");
                return;
            }

            int w = (int)capture.get(CAP_PROP_FRAME_WIDTH);
            int h = (int)capture.get(CAP_PROP_FRAME_HEIGHT);
            System::Drawing::Bitmap^ bmp = gcnew System::Drawing::Bitmap(w, h, System::Drawing::Imaging::PixelFormat::Format24bppRgb);
            System::Drawing::Rectangle rect(0, 0, w, h);

            isRunning = true;
            this->startbutton->Text = L"Stop";
            this->stopbutton->Enabled = true;
            this->roiButton->Enabled = false;

            while (isRunning) {
                auto bmpData = bmp->LockBits(rect, System::Drawing::Imaging::ImageLockMode::WriteOnly, bmp->PixelFormat);
                Mat frame(h, w, CV_8UC3, bmpData->Scan0.ToPointer(), bmpData->Stride);

                if (!capture.read(frame)) {
                    isRunning = false;
                    bmp->UnlockBits(bmpData);
                    break;
                }

                flip(frame, frame, 1);

                if (hasROI) {
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

                        // เรียกฟังก์ชันนับนิ้วที่ผ่าน Filter แล้ว
                        int result = CountFingers(roiProcess);

                        rectangle(frame, cv::Point(x1, y1), cv::Point(x2, y2), Scalar(0, 255, 0), 3);
                        cv::putText(frame, "Fingers: " + std::to_string(result),
                            cv::Point(x1, y1 - 10), cv::FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 255, 0), 2);
                    }
                }

                bmp->UnlockBits(bmpData);
                this->pictureBox1->Image = bmp;
                Application::DoEvents();
            }

            capture.release();
            cv::destroyAllWindows();
            this->startbutton->Text = L"Start";
            this->stopbutton->Enabled = false;
            this->roiButton->Enabled = true;
        }
        catch (std::exception& ex) { MessageBox::Show(gcnew System::String(ex.what()), "Error"); isRunning = false; }
    }

    System::Void MyForm::stopbutton_Click(System::Object^ sender, System::EventArgs^ e) { isRunning = false; }

    System::Void MyForm::roiButton_Click(System::Object^ sender, System::EventArgs^ e) {
        if (hasROI) {
            hasROI = false;
            this->roiButton->Text = L"Set ROI";
            this->Text = L"Camera Application";
        }
        else {
            if (isRunning) { MessageBox::Show("กรุณา Stop กล้องก่อนวาด ROI", "คำแนะนำ"); return; }
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