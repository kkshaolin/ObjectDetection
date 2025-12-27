import cv2 

img = cv2.imread("img_test\\WIN_25681227_22_58_45_Pro.jpg")
model = cv2.CascadeClassifier("haarcascade_frontalface_default.xml")

gray_scale = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
hand = model.detectMultiScale(gray_scale)

for (x,y,w,h) in hand:
    cv2.rectangle(img,(x,y),(x+w,y+h),(255,0,0),2)

cv2.imshow("image",img)
cv2.waitKey(0)
cv2.destroyAllWindows()