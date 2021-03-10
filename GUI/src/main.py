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
        self.tabs.addTab(self.tab2,"Upload Presets")
        
        ##########################
        # First tab
        ##########################

        self.tab1.layout = QVBoxLayout(self.tab1)

        # SD CARD PATH  

        self.tab1.cardLabel= QLabel("Path to SD card:")
        self.tab1.layout.addWidget(self.tab1.cardLabel)

        self.tab1.cardPath = QLineEdit()
        self.tab1.cardPath.textChanged.connect(self.t1CardPathChanged)
        self.tab1.layout.addWidget(self.tab1.cardPath)

        # SCROLL AREA
        self.tab1.scrollLayout = QVBoxLayout()
        self.tab1.groupBox = QGroupBox()
        self.tab1.groupBox.setLayout(self.tab1.scrollLayout)
        self.tab1.scroll = QScrollArea()
        self.tab1.scroll.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.tab1.scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.tab1.scroll.setWidgetResizable(True)
        self.tab1.scrollBuffer = QLabel()

        # DATA PATH

        self.tab1.saveLabel = QLabel("Path to save:")
        self.tab1.scrollLayout.addWidget(self.tab1.saveLabel)

        self.tab1.radioButton1 = QRadioButton("Use default")
        self.tab1.radioButton1.setChecked(True)
        self.tab1.radioButton1.toggled.connect(self.tab1Select)
        self.tab1.scrollLayout.addWidget(self.tab1.radioButton1)

        self.tab1.radioButton2 = QRadioButton("Use custom")
        self.tab1.radioButton2.toggled.connect(self.tab1Select)
        self.tab1.scrollLayout.addWidget(self.tab1.radioButton2)

        self.tab1.dataPath = QLineEdit()
        self.tab1.scrollLayout.addWidget(self.tab1.dataPath)
        self.tab1.scrollLayout.addWidget(self.tab1.scrollBuffer,60)   
        self.tab1Select() 

        self.tab1.layout.addWidget(self.tab1.groupBox)

        # PUSH BUTTON TO EXECUTE

        self.tab1.enter = QPushButton("Ok")
        self.tab1.enter.clicked.connect(self.offloadData)
        self.tab1.layout.addWidget(self.tab1.enter)

        # STATUS TEXT

        self.tab1.status = QLabel("")
        self.tab1.layout.addWidget(self.tab1.status)

        ##########################
        # Second tab
        ##########################

        self.tab2.layout = QVBoxLayout(self.tab2)

        # SD CARD PATH

        self.tab2.cardLabel = QLabel("Path to SD card:")
        self.tab2.cardPath = QLineEdit()
        self.tab2.cardPath.textChanged.connect(self.t2CardPathChanged)

        self.tab2.scroll = QScrollArea()

        self.tab2.scrollLayout = QFormLayout(self.tab2.scroll)
        self.tab2.scrollLayout.setLabelAlignment(Qt.AlignLeft)

        # PRESET OPTIONS

        self.tab2.radioButton1 = QRadioButton("Upload existing")
        self.tab2.radioButton1.setChecked(True)
        self.tab2.presetDropdown = QComboBox()
        self.tab2.presetDropdown.setSizeAdjustPolicy(QComboBox.AdjustToContents)
        self.tab2.presetDropdown.addItem("")
        self.tab2.presetDropdown.addItems(alt.preset_files)

        self.tab2.radioButton2= QRadioButton("Generate new")

        self.tab2.radioButton1.toggled.connect(self.tab2Select)
        self.tab2.radioButton2.toggled.connect(self.tab2Select)

        self.tab2.scrollLayout.addRow(self.tab2.radioButton1,self.tab2.presetDropdown)
        self.tab2.scrollLayout.addRow(self.tab2.radioButton2,QLabel(""))

        # SCROLL AREA
        self.tab2.groupBox = QGroupBox()
        self.tab2.groupBox.setLayout(self.tab2.scrollLayout)
        self.tab2.scroll.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOn)
        self.tab2.scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.tab2.scroll.setWidgetResizable(True)

        self.tab2.char = []
        self.tab2.char.append(QLineEdit())
        self.tab2.char[0].setMaxLength(20)
        self.tab2.char.append(QLineEdit())
        self.tab2.char[1].setMaxLength(11)
        self.tab2.char.append(QLineEdit())
        self.tab2.char[2].setMaxLength(11)

        self.tab2.scrollLayout.addRow(QLabel("Survey name"),self.tab2.char[0])
        self.tab2.scrollLayout.addRow(QLabel("Zero point"),self.tab2.char[1])
        for x in range(1,51):
            self.tab2.char.append(QLineEdit())
            self.tab2.char[x+1].setMaxLength(11)
            self.tab2.scrollLayout.addRow(QLabel("Point "+str(x)),self.tab2.char[x+1])

        self.tab2.scroll.setWidget(self.tab2.groupBox)

        self.tab2.enter = QPushButton("Ok")
        self.tab2.enter.clicked.connect(self.createPreset)

        self.tab2.status = QLabel("")

        # Final SETUP TAB 2
        self.tab2Select()
        
        self.tab2.layout.addWidget(self.tab2.cardLabel)
        self.tab2.layout.addWidget(self.tab2.cardPath)
        self.tab2.layout.addWidget(self.tab2.scroll)
        self.tab2.layout.addWidget(self.tab2.enter)
        self.tab2.layout.addWidget(self.tab2.status)
        
        # Add tabs to widget
        self.layout.addWidget(self.tabs)
        
    @pyqtSlot()
    def on_click(self):
        print("\n")
        for currentQTableWidgetItem in self.tableWidget.selectedItems():
            print(currentQTableWidgetItem.row(), currentQTableWidgetItem.column(), currentQTableWidgetItem.text())

    def t1CardPathChanged(self):
        self.tab2.cardPath.blockSignals(True)
        self.tab2.cardPath.setText(self.tab1.cardPath.text())
        self.tab2.cardPath.blockSignals(False)

    def t2CardPathChanged(self):
        self.tab1.cardPath.blockSignals(True)
        self.tab1.cardPath.setText(self.tab2.cardPath.text())
        self.tab1.cardPath.blockSignals(False)

    def createPreset(self):
        alt.set_card_path(self.tab2.cardPath.text())
        if (self.tab2.radioButton1.isChecked()): # upload existing file
            alt.preset_file_name = self.tab2.presetDropdown.currentText() + ".txt"
            success = alt.upload_existing_preset()
        else:  # generate new file
            self.tab2.presetDropdown.addItem(self.tab2.char[0].text())
            for x in range(52):
                if (self.tab2.char[x].text() != ""):
                    alt.preset_input.append(self.tab2.char[x].text())
                    self.tab2.char[x].setText("")
            success = alt.generate_preset()
        if (success == -1):
            self.tab2.status.setText("SD card path does not exist")
        elif (success == -2):
            self.tab2.status.setText("Select an existing preset file")

        else:
            self.tab2.status.setText("Success")

    def tab1Select(self):
        if self.tab1.radioButton1.isChecked():
            self.tab1.dataPath.setReadOnly(True)
        else:
            self.tab1.dataPath.setReadOnly(False)

    def tab2Select(self):
        if self.tab2.radioButton1.isChecked():
            self.tab2.presetDropdown.setEnabled(True)
            for x in range(52):
                self.tab2.char[x].setReadOnly(True)
        else:
            self.tab2.presetDropdown.setEnabled(False)
            for x in range(52):
                self.tab2.char[x].setReadOnly(False)

    def offloadData(self):
        alt.set_card_path(self.tab1.cardPath.text())
        if (self.tab1.radioButton1.isChecked()): 
            alt.data_path = alt.data_path_default
        else: 
            alt.data_path = alt.set_data_path(self.tab1.dataPath.text())

        success = alt.offload_data()

        if (success == -1):
            self.tab1.status.setText("SD card path does not exist")
        elif (success == -2):
            self.tab1.status.setText("Save destination does not exist")
        else:
            self.tab1.status.setText("Success")





        
if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = App()
    sys.exit(app.exec_())