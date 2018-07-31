import os
import cv2
import time
import argparse
import multiprocessing
import numpy as np
import tensorflow as tf
import serial
from PIL import Image


from utils.app_utils import FPS, WebcamVideoStream
from multiprocessing import Queue, Pool
from object_detection.utils import label_map_util
from object_detection.utils import visualization_utils as vis_util
green_min = np.array([35, 128, 46])
green_max = np.array([77, 255, 255])

CWD_PATH = os.getcwd()

# Path to frozen detection graph. This is the actual model that is used for the object detection.
MODEL_NAME = 'ssd_mobilenet_v1_coco_11_06_2017'
PATH_TO_CKPT = os.path.join(CWD_PATH, 'object_detection', MODEL_NAME, 'frozen_inference_graph.pb')

# List of the strings that is used to add correct label for each box.
PATH_TO_LABELS = os.path.join(CWD_PATH, 'object_detection', 'data', 'rp_label_map.pbtxt')

NUM_CLASSES = 1

# Loading label map
label_map = label_map_util.load_labelmap(PATH_TO_LABELS)
categories = label_map_util.convert_label_map_to_categories(label_map, max_num_classes=NUM_CLASSES,
                                                            use_display_name=True)
category_index = label_map_util.create_category_index(categories)


def detect_objects(image_np, sess, detection_graph):
    # Expand dimensions since the model expects images to have shape: [1, None, None, 3]
    image_np_expanded = np.expand_dims(image_np, axis=0)
    image_tensor = detection_graph.get_tensor_by_name('image_tensor:0')

    # Each box represents a part of the image where a particular object was detected.
    boxes = detection_graph.get_tensor_by_name('detection_boxes:0')

    # Each score represent how level of confidence for each of the objects.
    # Score is shown on the result image, together with the class label.
    scores = detection_graph.get_tensor_by_name('detection_scores:0')
    classes = detection_graph.get_tensor_by_name('detection_classes:0')
    num_detections = detection_graph.get_tensor_by_name('num_detections:0')

    # Actual detection.
    (boxes, scores, classes, num_detections) = sess.run(
        [boxes, scores, classes, num_detections],
        feed_dict={image_tensor: image_np_expanded})

    # Visualization of the results of a detection.
    vis_util.visualize_boxes_and_labels_on_image_array(
        image_np,
        np.squeeze(boxes),
        np.squeeze(classes).astype(np.int32),
        np.squeeze(scores),
        category_index,
        use_normalized_coordinates=True,
        line_thickness=8)
    return image_np


def worker(input_q, mid_q):
    # Load a (frozen) Tensorflow model into memory.
    detection_graph = tf.Graph()
    with detection_graph.as_default():
        od_graph_def = tf.GraphDef()
        with tf.gfile.GFile(PATH_TO_CKPT, 'rb') as fid:
            serialized_graph = fid.read()
            od_graph_def.ParseFromString(serialized_graph)
            tf.import_graph_def(od_graph_def, name='')

        sess = tf.Session(graph=detection_graph)

    fps = FPS().start()
    while True:
        fps.update()
        frame = input_q.get()
        frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        aaa=detect_objects(frame_rgb, sess, detection_graph)
        #time.sleep(0.3)
        #############################图像处理
        
        
####################################
        mid_q.put(aaa)

    fps.stop()
    sess.close()
def worker2(mid_q,output_q):
    fps = FPS().start()
    ser = serial.Serial('/dev/tty.HC-05-DevB') # 注意选择串口号
    go="1\n"
    turn_right="2\n"
    stop="3\n"
    turn_left="4\n"
    back="5\n"
    hand_put="6\n"
    hand_catch="7\n"
    go=go.encode('UTF-8')
    turn_right=turn_right.encode('UTF-8')
    stop=stop.encode('UTF-8')
    turn_left=turn_left.encode('UTF-8')
    back=back.encode('UTF-8')
    hand_put=hand_put.encode('UTF-8')
    hand_catch=hand_catch.encode('UTF-8')
    mid_loss=0
    pri_loss=0
    t_print=0
    while True:
        fps.update()
        lb = output_q.get()
        hsv = cv2.cvtColor(lb, cv2.COLOR_BGR2HSV)
        mask=cv2.inRange(hsv,  green_min,  green_max)
        res=cv2.bitwise_and(lb, lb, mask=mask)
        kernel = np.ones((5,5),np.uint8)
        kernel2 = np.ones((3,3),np.uint8)
        dst = cv2.morphologyEx(res, cv2.MORPH_OPEN, kernel)
        res = cv2.erode(dst,kernel2,iterations = 3)
        edges=cv2.Canny(res,0,30,3)
        edges=np.array(edges)
        im = Image.fromarray(edges)
        width,height=im.size
        row_num = np.zeros(width)
        #line_num = np.zeros(height)
        #line_index=np.zeros(4)
        row_index=np.zeros(4)
        row_c=0
    #line_c=0
        for i in range(0,height):
            for j in range(0,width):
                if edges[i,j]>100:
                    #line_num[i]=line_num[i]+1
                    row_num[j]=row_num[j]+1
    
        for i in range(0,4):
            
            ac=row_num.argmax(axis=0)
            if row_num[ac]>30:
                row_index[i]=ac
                row_num[ac]=0
                row_c=row_c+1
#           ab=line_num.argmax(axis=0)
#
#            if line_num[ab]>30:
#                line_index[i]=ab
#                line_num[ab]=0
#                line_c=line_c+1
        if row_c>0:
            row_mid=(int)((np.mean(row_index))*4/row_c)
        else:
            row_mid=np.mean(row_index)

        row_mid_de=int(width/2)
        pri_loss=mid_loss
        mid_loss=row_mid-row_mid_de   #大于0，图像在中点右边；小于0，图像在中点左边
        if time.time()-t_print > 1.8:
            t_print=time.time()
            if mid_loss>30:
                ser.write(turn_right)
                print("R")
            elif mid_loss<(-30):
                ser.write(turn_left)
                print("L")
            elif mid_loss>-30 or mid_loss<30:
                ser.write(go)
                print("正了")

    #time.sleep(0.3)

    fps.stop()
    sess.close()


if __name__ == '__main__':
    

    logger = multiprocessing.log_to_stderr()
    logger.setLevel(multiprocessing.SUBDEBUG)
    

    input_q = Queue(maxsize=5)
    output_q = Queue(maxsize=5)
    mid_q=Queue(maxsize=5)
    pool = Pool(3, worker, (input_q, mid_q))
    pool2 = Pool(3, worker2, (mid_q, output_q))
    # webcamera

    cam_url='http://192.168.8.1:8083/?action=stream'
    video_capture = WebcamVideoStream(src=cam_url,width=640,height=480).start()
#video_capture=cv2.VideoCapture(0)
    
    fps = FPS().start()
    t_start = time.time()
    out = None
    while True:  # fps._numFrames < 120
        if time.time()-t_start > 1.2: # 输出图像的延迟
            t_start = time.time()
            frame = video_capture.read()
            frame=cv2.resize(frame,(int(640),int(480)))
            input_q.put(frame)
            
            t = time.time()
            out = mid_q.get()
            output_q.put(out)
            cv2.imshow('Video', out)
            fps.update()
            print('[INFO] elapsed time: {:.2f}'.format(time.time() - t))
        else:
            frame = video_capture.read()
            if out is not None:
                cv2.imshow("Video", out)
            else:
                cv2.imshow("Video", frame)
        
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
# time.sleep(0.3)

