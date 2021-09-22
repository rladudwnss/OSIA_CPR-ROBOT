import cv2                 #opencv
import numpy as np         #영상을 다루기 위해 배열 numpy
import serial              #시리얼 통신을 위함
import os                  #운영체제 제어를 위함
from socket import*        #소켓통신을 위함
from threading import*     #object detection과 통신을 위해 쓰레딩


PORT = 'COM7'                       #아두이노 포트
host = "192.168.0.213"  # host ip #"192.168.0.213"
Port = 54321  # 시리얼 통신에 대한 초기설정
SER = serial.Serial(PORT, 9600)
cam_on_flag = 3
serverSocket = socket(AF_INET, SOCK_STREAM)
serverSocket.bind((host, Port))
serverSocket.listen(1)
print("wait")
(connectionSocket, addr) = serverSocket.accept()
print(str(addr), "from connect.")  # connect check

# server setting #


def camfunc():
    cap = cv2.VideoCapture(0)
    #print('실행되었음')
    while (True):
        if SER.readable():
            #print('if문탔음')
            cap = cv2.VideoCapture(0)
            res = SER.readline()
            print(res.decode()[:len(res) - 1])
            cam_on_flag = int(res.decode()[:len(res) - 1])
            # if res.decode() == 0x35:
            #     cam_on_flag = 1
        if cam_on_flag == 5:
            ret, frame = cap.read()
            if not ret:
                print("error!")
                break
            gray_img = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            rat, edge_img = cv2.threshold(gray_img, 70, 255, cv2.THRESH_BINARY)
            cv2.imshow('src', edge_img)
            count_pixel = cv2.countNonZero(edge_img)
            print('픽셀수 : ', count_pixel)
            if cv2.waitKey(1) == ord('q'):
                break
            if count_pixel < 10000:  # 투명PET
                cap.release()
                cv2.destroyAllWindows()
                SER.write(([1]))
                cam_on_flag = 0
                message = (str("no label clear PET"))  # message를 입력해서 보내줌
                connectionSocket.send(message.encode())  # messagem를 보낼때에는  encode함수 사용
            elif count_pixel > 10000 and count_pixel < 40000:  # 투명 + 라벨지 PET
                cap.release()
                cv2.destroyAllWindows()
                SER.write([2])
                cam_on_flag = 0
                message = (str("label  clear PET"))  # message를 입력해서 보내줌
                connectionSocket.send(message.encode())  # messagem를 보낼때에는  encode함수 사용
            elif count_pixel > 40000:  # 유색PET
                cap.release()
                cv2.destroyAllWindows()
                SER.write([3])
                cam_on_flag = 0
                message = (str("color PET"))  # message를 입력해서 보내줌
                connectionSocket.send(message.encode())  # messagem를 보낼때에는  encode함수 사용


        elif cam_on_flag == 0:
            print('대기하시오.')
            # cap = cv2.VideoCapture(0)
    cap.release()
    cv2.destroyAllWindows()

def serverfunc():
    # 스레드를 사용하여 소켓통신을 하기 위해 만든 함수이다.

    # host에 대한 ip와 해당하는 포트 번호를 열어주었다. -> 포트포워딩을 하였고, 이 코드는 서버 코드이다.
    ## server setting ##
    # 휴대폰 어플을 통해 접속하여, 해당하는 버튼을 눌렀을 경우, 그에 맞는 신호를 아두이노에 보내주는 반복문 + 조건문들 이다.
    while (True):
        # if data=='d':
        #   print("recv data:", data.decode("utf-8"))
        # print("recv data:", data.decode("utf-8"))z
        #connectionSocket.sendall("hello")

        #message = input(str("plz enter your msg :")) #message를 입력해서 보내줌
        #connectionSocket.send(message.encode())      #messagem를 보낼때에는  encode함수 사용

        data = connectionSocket.recv(1024)
        if data.decode("utf-8") == 'm':             #manual mode on
            SER.write([20])   #auto mode serial number send
            print("recv data:", data.decode("utf-8"))
        elif data.decode("utf-8") == 'a':        #auto mode on
            SER.write([10])   #manual mode on
            print("recv data:", data.decode("utf-8"))
        #xd (x축 버튼 down) xu(x축 버튼 up), xcd(x축 반시계방향 버튼 down), xcu(x축 반시계방향 버튼 up)##
        elif data.decode("utf-8") == 'xd':  # xcw x축 시계방향 버튼이 눌렸을 때
            print("recv data:", data.decode("utf-8"))  #XU 일 경우 모터계속 구동
            SER.write([11])  # right turn serial number '1'
        elif data.decode("utf-8") == 'xu':  # xcw x축 시계방향 버튼이 떼질때
            print("recv data:", data.decode("utf-8"))
            SER.write([4])  # 'all stop' serial number  '4'
        elif data.decode("utf-8") == 'xcd':  # xccw x축 반시계 방향 버튼이 눌렀을떄
            print("recv data:", data.decode("utf-8"))
            SER.write([12])  # backward serial number '2'
        elif data.decode("utf-8") == 'xcu':  # xccw멈춤, x축 반시계 방향 버튼이 떼졌을때
            print("recv data:", data.decode("utf-8"))
            SER.write([4])  # 'all stop' serial number  '4'
         #-------------------------------------------------------------------#
        elif data.decode("utf-8") == 'yd':  # ycw  y축 시계방향 버튼이 눌렸을 때
            print("recv data:", data.decode("utf-8"))
            SER.write([21])  # auto mode serial number '21'
        elif data.decode("utf-8") == 'yu':  # ycw멈춤
            print("recv data:", data.decode("utf-8"))
            SER.write([4])  # 'all stop' serial number  '4'
        elif data.decode("utf-8") == 'ycd':  # yccw
            print("recv data:", data.decode("utf-8"))
            SER.write([22])  # stop mode serial  mode number '22'
        elif data.decode("utf-8") == 'ycu':  # yccw멈춤
            print("recv data:", data.decode("utf-8"))
            SER.write([4])  # 'all stop' serial number  '4'
        # -------------------------------------------------------------------#
        elif data.decode("utf-8") == 'zud':  # z축이 up버튼 눌렸을때
            print("recv data:", data.decode("utf-8"))
            SER.write([31])  # spray run serial number '10'
        elif data.decode("utf-8") == 'zuu':  # z축 up버튼이 떼졌을때
            print("recv data:", data.decode("utf-8"))
            SER.write([4])  # 'all stop' serial number  '4'
        elif data.decode("utf-8") == 'zdd':  # z축 down 버튼이 눌렸을때
            print("recv data:", data.decode("utf-8"))
            SER.write([32])  # spray run serial number '10'
        elif data.decode("utf-8") == 'zdu':  # z축 down 버튼이 떼졌을때
            print("recv data:", data.decode("utf-8"))
            SER.write([4])  # 'all stop' serial number  '4'
        # -------------------------------------------------------------------#
        elif data.decode("utf-8") == 'wd':  # gripper close 버튼이 떼졌을 때
            print("recv data:", data.decode("utf-8"))
            SER.write([41])  # spray run serial number '10'
        elif data.decode("utf-8") == 'wu':  # gripper close 버튼이 떼졌을 때
            print("recv data:", data.decode("utf-8"))
            SER.write([4])  # 'all stop' serial number  '4'
        elif data.decode("utf-8") == 'wcd':  # gripper close 버튼이 떼졌을 때
            print("recv data:", data.decode("utf-8"))
            SER.write([42])  # spray run serial number '10'
        elif data.decode("utf-8") == 'wcu':  # gripper close 버튼이 떼졌을 때
            print("recv data:", data.decode("utf-8"))
            SER.write([4])  # 'all stop' serial number  '4'
        # -------------------------------------------------------------------#
        elif data.decode("utf-8") == 'god':  # gripper  open 버튼이 눌렸을 때
            print("recv data:", data.decode("utf-8"))
            SER.write([51])  # spray run serial number '10'
        elif data.decode("utf-8") == 'gou':  # gripper  open 버튼이 떼졌을 때
            print("recv data:", data.decode("utf-8"))
            SER.write([4])  # 'all stop' serial number  '4'
        elif data.decode("utf-8") == 'gcd':  # gripper close 버튼이 눌렸을 때
            print("recv data:", data.decode("utf-8"))
            SER.write([52])  # spray run serial number '10'
        elif data.decode("utf-8") == 'gcu':  # gripper close 버튼이 떼졌을 때
            print("recv data:", data.decode("utf-8"))
            SER.write([4])  # 'all stop' serial number  '4'
         # -------------------------------------------------------------------#
        elif data.decode("utf-8") == 'stop':  # 비상 정지 버튼 -> break
            print("recv data:", data.decode("utf-8"))
            SER.write([44])  # Arduino roop break serial num '44'

    serverSocket.close()

#스레드를 실행시키는 문장
if __name__ == '__main__':
    proc = Thread(target=camfunc, args=())
    proc2 = Thread(target=serverfunc, args=())
    proc.start()
    proc2.start()
    #proc2.join()         #serverfunc이 다 돌면 쓰레딩을 종료시킨다.
