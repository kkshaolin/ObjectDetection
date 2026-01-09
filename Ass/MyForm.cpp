#include "MyForm.h"
#include "showout.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

using namespace cv;
using namespace System::Drawing;

namespace Ass {

    enum GestureType {

        GESTURE_NONE = 0,

        GESTURE_1 = 1,

        GESTURE_2 = 2,

        GESTURE_3 = 3,

        GESTURE_4 = 4,

        GESTURE_5 = 5,

        GESTURE_loveu = 6,

        GESTURE_BYE = 7 

    };

    

    int MyForm::CountFingers(const cv::Mat& roi) {



        static std::vector<int> fingerHistory;

        static std::vector<int> xHistory; // à¡çº»ÃÐÇÑµÔµÓáË¹è§á¡¹ X ¢Í§¨Ø´ÈÙ¹Âì¡ÅÒ§Á×Í

        const int MAX_HISTORY = 17;

        const int MOTION_HISTORY = 10; // à¡çº»ÃÐÇÑµÔ¡ÒÃ¢ÂÑº 10 à¿ÃÁÅèÒÊØ´



        if (roi.empty()) return GESTURE_NONE;



        cv::Mat ycrcb, mask;

        

        cv::cvtColor(roi, ycrcb, cv::COLOR_BGR2YCrCb);



        

        cv::inRange(ycrcb, cv::Scalar(0, 133, 77), cv::Scalar(255, 173, 127), mask);



        

        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7));

        cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

        

        cv::GaussianBlur(mask, mask, cv::Size(5, 5), 0);

        cv::threshold(mask, mask, 127, 255, cv::THRESH_BINARY);



        

        cv::imshow("Debug: Processed Mask", mask);

        cv::waitKey(1);



        std::vector<std::vector<cv::Point>> contours;

        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);



        if (contours.empty()) return GESTURE_NONE;



        int largestIdx = -1;

        double maxArea = 0;

        for (size_t i = 0; i < contours.size(); i++) {

            double area = cv::contourArea(contours[i]);

            if (area > maxArea) {

                maxArea = area;

                largestIdx = (int)i;

            }

        }



        if (largestIdx == -1 || maxArea < 3000) return GESTURE_NONE;



        // --- à¾ÔèÁ: ¤Ó¹Ç³ Centroid áÅÐ¡ÒÃà¤Å×èÍ¹·Õèá¹Ç¹Í¹ (ÊÓËÃÑºµÃÇ¨âº¡Á×Í) ---

        cv::Moments m = cv::moments(contours[largestIdx]);

        int centerX = (m.m00 != 0) ? (int)(m.m10 / m.m00) : 0;

        xHistory.push_back(centerX);

        if (xHistory.size() > MOTION_HISTORY) xHistory.erase(xHistory.begin());



        int movement = 0;

        if (xHistory.size() >= 2) {

            int minX = *std::min_element(xHistory.begin(), xHistory.end());

            int maxX = *std::max_element(xHistory.begin(), xHistory.end());

            movement = maxX - minX;

        }



        std::vector<int> hullIndices;

        cv::convexHull(contours[largestIdx], hullIndices, false);



        int validDefects = 0;

        double maxDistanceBetweenFingers = 0; // à¾ÔèÁµÑÇá»Ãàªç¤ÃÐÂÐËèÒ§



        if (hullIndices.size() > 3) {

            std::vector<cv::Vec4i> defects;

            try {

                cv::convexityDefects(contours[largestIdx], hullIndices, defects);

                for (const auto& defect : defects) {

                    float depth = defect[3] / 256.0f;

                    cv::Point pStart = contours[largestIdx][defect[0]];

                    cv::Point pEnd = contours[largestIdx][defect[1]];

                    cv::Point pFar = contours[largestIdx][defect[2]];



                    if (depth > 40.0f) {

                        double a = std::sqrt(std::pow(pEnd.x - pStart.x, 2) + std::pow(pEnd.y - pStart.y, 2));

                        double b = std::sqrt(std::pow(pFar.x - pStart.x, 2) + std::pow(pFar.y - pStart.y, 2));

                        double c = std::sqrt(std::pow(pFar.x - pEnd.x, 2) + std::pow(pFar.y - pEnd.y, 2));

                        double denom = 2 * b * c;

                        double angle = 180.0; // default large

                        if (denom > 1e-6) {

                            double cosv = (b * b + c * c - a * a) / denom;

                            if (cosv > 1.0) cosv = 1.0;

                            if (cosv < -1.0) cosv = -1.0;

                            angle = std::acos(cosv) * 57.2958;

                        }



                        if (angle < 90) {

                            validDefects++;

                            if (a > maxDistanceBetweenFingers) maxDistanceBetweenFingers = a;

                        }

                    }

                }

            }

            catch (...) { return GESTURE_NONE; }

        }



        

        int rawResult = GESTURE_NONE;

        int fingerCount = validDefects + 1;



        if (fingerCount >= 5) {

            

            if (movement > 30) {

                rawResult = GESTURE_BYE;

            }
            else {

                rawResult = GESTURE_5;

            }

        }

        else if (fingerCount == 3) {

            

            if (maxDistanceBetweenFingers > 100) rawResult = GESTURE_loveu;

            else rawResult = GESTURE_3;

        }

        else {

            if (fingerCount == 1) rawResult = GESTURE_1;

            else if (fingerCount == 2) rawResult = GESTURE_2;

            else if (fingerCount == 4) rawResult = GESTURE_4;

            else if (fingerCount >= 5) rawResult = GESTURE_5;

        }



        

        fingerHistory.push_back(rawResult);

        if (fingerHistory.size() > MAX_HISTORY) fingerHistory.erase(fingerHistory.begin());



        std::map<int, int> counts;

        for (int v : fingerHistory) counts[v]++;



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



    // --- ÊèÇ¹ËÅÑ¡: startbutton_Click ---

    System::Void MyForm::startbutton_Click(System::Object^ sender, System::EventArgs^ e) {
        if (isRunning) { isRunning = false; return; }
        //MessageBox::Show("Path ปัจจุบันคือ:\n" + System::IO::Directory::GetCurrentDirectory());
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

            // --- ส่วนที่เพิ่ม 1: ตัวแปรสำหรับจัดการหน้าต่าง Showout ---
            Ass::showout^ stickerForm = nullptr; // เก็บหน้าต่างสติ๊กเกอร์
            int lastGesture = -1;                // เก็บค่าท่าทางก่อนหน้า
            // -----------------------------------------------------

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

                        int result = CountFingers(roiProcess);

                        std::string label = "";
                        System::String^ stickerUrl = ""; // ตัวแปรเก็บ URL รูป

                        // --- ส่วนที่แก้ไข 2: ใน Switch แค่กำหนดค่า String ห้าม ShowDialog ---
                        switch (result) {
                        case GESTURE_1:
                            label = "1: Number 1";
                            stickerUrl = ".\\image\\1.png";
                            break;
                        case GESTURE_2:
                            label = "2: Number 2";
                            stickerUrl = ".\\image\\2.png";
                            break;
                        case GESTURE_3:
                            label = "3: Number 3";
                            stickerUrl = ".\\image\\3.png";
                            break;
                        case GESTURE_4:
                            label = "4: Number 4";
                            stickerUrl = ".\\image\\4.jpg";
                            break;
                        case GESTURE_5:
                            label = "5: Number 5";
                            stickerUrl = ".\\image\\5.jpg";
                            break;
                        case GESTURE_loveu:
                            label = "Love U";
                            stickerUrl = ".\\image\\love.jpg";
                            break;
                        case GESTURE_BYE:
                            label = "BYE BYE: Waving Hand";
                            stickerUrl = ".\\image\\bye.jpg";
                            break;
                        case GESTURE_NONE:
                            label = "Searching...";
                            stickerUrl = ""; // ไม่แสดงรูป
                            break;
                        default:
                            label = "Unknown";
                            stickerUrl = "";
                            break;
                        }

                        // --- ส่วนที่เพิ่ม 3: Logic ควบคุมหน้าต่าง Showout แบบ Realtime ---

                        // ตรวจสอบว่าท่าทางเปลี่ยนไปจากรอบที่แล้วหรือไม่
                        if (result != lastGesture) {

                            // ถ้ามีหน้าต่างเก่าเปิดอยู่ ให้ปิดไปก่อน
                            if (stickerForm != nullptr && !stickerForm->IsDisposed) {
                                stickerForm->Close();
                                stickerForm = nullptr;
                            }

                            // ถ้าท่าทางใหม่ไม่ใช่ NONE และมี URL ให้สร้างหน้าต่างใหม่
                            if (result != GESTURE_NONE && stickerUrl != "") {
                                stickerForm = gcnew Ass::showout(stickerUrl);

                                // ตั้งค่าให้หน้าต่างอยู่บนสุดเสมอ (Optional)
                                stickerForm->TopMost = true;

                                // ใช้ Show() แทน ShowDialog() เพื่อไม่ให้โปรแกรมค้าง
                                stickerForm->Show();
                            }

                            lastGesture = result; // จำท่าทางปัจจุบันไว้เทียบรอบหน้า
                        }
                        // -----------------------------------------------------------

                        rectangle(frame, cv::Point(x1, y1), cv::Point(x2, y2), Scalar(0, 255, 0), 3);
                        cv::putText(frame, label,
                            cv::Point(x1, y1 - 10), cv::FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 255, 0), 2);
                    }
                }

                bmp->UnlockBits(bmpData);
                this->pictureBox1->Image = bmp;
                Application::DoEvents(); // สำคัญมาก: ทำให้ UI ตอบสนองและวาดหน้าต่าง
            }

            // --- ส่วนที่เพิ่ม 4: Clean up เมื่อกด Stop หรือจบลูป ---
            if (stickerForm != nullptr && !stickerForm->IsDisposed) {
                stickerForm->Close();
                stickerForm = nullptr;
            }
            // ------------------------------------------------

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