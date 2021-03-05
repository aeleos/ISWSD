# Filename : main.py

# main and GUI for ISWSD

import sys
from PyQt5.QtWidgets import QMainWindow, QApplication, QPushButton, QWidget, QAction, QTabWidget,QVBoxLayout,QRadioButton,QLabel,QLineEdit
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import pyqtSlot

class App(QMainWindow):

    def __init__(self):
        super().__init__()
        self.title = 'Survey Altimeter Data Manager'
        self.left = 0
        self.top = 0
        self.width = 300
        self.height = 200
        self.setWindowTitle(self.title)
        self.setGeometry(self.left, self.top, self.width, self.height)
        
        self.table_widget = MyTableWidget(self)
        self.setCentralWidget(self.table_widget)
        
        self.show()
    
class MyTableWidget(QWidget):
    
    def __init__(self, parent):
        super(QWidget, self).__init__(parent)
        self.layout = QVBoxLayout(self)
        
        # Initialize tab screen
        self.tabs = QTabWidget()
        self.tab1 = QWidget()
        self.tab2 = QWidget()
        self.tabs.resize(300,200)
        
        # Add tabs
        self.tabs.addTab(self.tab1,"Offload Data")
        self.tabs.addTab(self.tab2,"Upload Custom")
        
        # Create first tab
        self.tab1.layout = QVBoxLayout(self)

        self.label = QLabel("Select file path")
        self.tab1.layout.addWidget(self.label)

        self.radiobutton1 = QRadioButton("Use default")
        self.radiobutton1.setChecked(True)
        self.tab1.layout.addWidget(self.radiobutton1)

        self.radiobutton2 = QRadioButton("Use custom")
        self.tab1.layout.addWidget(self.radiobutton2)

        self.filepath = QLineEdit()
        self.filepath.setReadOnly(self.radiobutton1.isChecked())
        self.tab1.layout.addWidget(self.filepath)

        self.pushButton1 = QPushButton("Ok")
        self.tab1.layout.addWidget(self.pushButton1)

        self.tab1.setLayout(self.tab1.layout)
        
        # Add tabs to widget
        self.layout.addWidget(self.tabs)
        self.setLayout(self.layout)
        
    @pyqtSlot()
    def on_click(self):
        print("\n")
        for currentQTableWidgetItem in self.tableWidget.selectedItems():
            print(currentQTableWidgetItem.row(), currentQTableWidgetItem.column(), currentQTableWidgetItem.text())

if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = App()
    sys.exit(app.exec_())