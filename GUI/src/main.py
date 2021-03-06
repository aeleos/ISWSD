# Filename : main.py

# main and GUI for ISWSD

import sys
from PyQt5.QtWidgets import QMainWindow,QApplication,QPushButton,QWidget,QAction,QTabWidget,QVBoxLayout,QRadioButton,QLabel,QLineEdit,QComboBox,QFormLayout,QScrollArea,QGroupBox
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import pyqtSlot,Qt

from Altimeter import *

alt = Altimeter()

class App(QMainWindow):

    def __init__(self):
        super().__init__()
        self.title = 'Survey Altimeter Data Manager'
        self.left = 0
        self.top = 0
        self.width = 500
        self.height = 400
        self.setWindowTitle(self.title)
        self.setGeometry(self.left, self.top, self.width, self.height)
        
        self.table_widget = MyTableWidget(self)
        self.setCentralWidget(self.table_widget)
        
        self.show()
    
class MyTableWidget(QWidget):
    
    def __init__(self, parent):
        super(QWidget, self).__init__(parent)
        self.layout = QVBoxLayout(self)
        ##########################
        # Initialize tab screen
        ##########################
        self.tabs = QTabWidget()
        self.tab1 = QWidget()
        self.tab2 = QWidget()
        self.tabs.resize(500,400)
        
        # Add tabs
        self.tabs.addTab(self.tab1,"Offload Data")
        self.tabs.addTab(self.tab2,"Upload Custom")
        
        ##########################
        # First tab
        ##########################

        self.tab1.layout = QVBoxLayout(self)

        # SD CARD PATH

        self.labelT1 = QLabel("Path to SD card:")
        self.tab1.layout.addWidget(self.labelT1)

        self.filepathT1 = QLineEdit()
        self.tab1.layout.addWidget(self.filepathT1)

        # DATA PATH

        self.label = QLabel("Path to save:")
        self.tab1.layout.addWidget(self.label)

        self.radiobutton1 = QRadioButton("Use default")
        self.radiobutton1.setChecked(True)
        self.tab1.layout.addWidget(self.radiobutton1)

        self.radiobutton2 = QRadioButton("Use custom")
        self.tab1.layout.addWidget(self.radiobutton2)

        self.filepath = QLineEdit()
        self.filepath.setReadOnly(self.radiobutton1.isChecked())
        self.tab1.layout.addWidget(self.filepath)

        # PUSH BUTTON TO EXECUTE

        self.pushButton1 = QPushButton("Ok")
        self.tab1.layout.addWidget(self.pushButton1)

        # STATUS TEXT

        self.tab1bottom = QLabel("Status here")
        self.tab1.layout.addWidget(self.tab1bottom)

        self.tab1.setLayout(self.tab1.layout)

        ##########################
        # Second tab
        ##########################

        self.tab2.buttonlayout = QVBoxLayout(self)

        # SD CARD PATH

        self.labelT2 = QLabel("Path to SD card:")
        self.filepathT2 = QLineEdit()

        self.tab2.layout = QFormLayout(self)
        self.tab2.layout.setLabelAlignment(Qt.AlignLeft)

        # CUSTOM OPTIONS

        self.radiobutton3 = QRadioButton("Upload existing")
        self.radiobutton3.setChecked(True)
        self.dropdown = QComboBox()
        self.dropdown.addItems(alt.custom_files)

        self.radiobutton4 = QRadioButton("Generate new")

        self.tab2.layout.addRow(self.radiobutton3,self.dropdown)
        self.tab2.layout.addRow(self.radiobutton4,QLabel(""))

        # SCROLL AREA
        self.groupBox = QGroupBox()
        self.groupBox.setLayout(self.tab2.layout)
        self.scroll = QScrollArea()
        self.scroll.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOn)
        self.scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.scroll.setWidgetResizable(True)

        self.char = []
        self.char.append(QLineEdit())
        self.char[0].setMaxLength(20)
        self.char.append(QLineEdit())
        self.char[1].setMaxLength(11)
        self.char.append(QLineEdit())
        self.char[2].setMaxLength(11)

        self.tab2.layout.addRow(QLabel("Survey name"),self.char[0])
        self.tab2.layout.addRow(QLabel("Zero point"),self.char[1])
        for x in range(1,51):
            self.char.append(QLineEdit())
            self.char[x+1].setMaxLength(11)
            self.tab2.layout.addRow(QLabel("Point "+str(x)),self.char[x+1])

        self.scroll.setWidget(self.groupBox)

        self.pushButton2 = QPushButton("Ok")

        self.tab2bottom = QLabel("Status here")

        # Final SETUP TAB 2
        
        self.tab2.buttonlayout.addWidget(self.labelT2)
        self.tab2.buttonlayout.addWidget(self.filepathT2)
        self.tab2.buttonlayout.addWidget(self.scroll)
        self.tab2.buttonlayout.addWidget(self.pushButton2)
        self.tab2.buttonlayout.addWidget(self.tab2bottom)
        self.tab2.setLayout(self.tab2.buttonlayout)
        # Add tabs to widget
        self.layout.addWidget(self.tabs)
        self.setLayout(self.layout)
        
    @pyqtSlot()
    def on_click(self):
        self.tab1.filepathT1.read
        print("\n")
        for currentQTableWidgetItem in self.tableWidget.selectedItems():
            print(currentQTableWidgetItem.row(), currentQTableWidgetItem.column(), currentQTableWidgetItem.text())


        
if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = App()
    sys.exit(app.exec_())