1.将图片放入image文件夹下
2.用labelImg工具标注图片：python labelImg.py
3.将xml文件放入annotations文件夹下
4.修改generate文件中的标注名，可以多加分类，return往下排即可
5.   python xml*
6.   python3 generate_tfrecord.py --csv_input=train_labels.csv --output_path=train_test.record
