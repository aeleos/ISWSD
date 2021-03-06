import os

CUST_PATH = "/cust/"
DATA_PATH = "/data/"

class Altimeter:

    def __init__(self):
        self.data_path_default = ""
        self.custom_path_default = ""
        self.data_path_custom = ""
        self.custom_path_custom = ""
        self.card_path = ""

        self.custom_files = []
        self.card_files = []
        self.data_files = []

        self.get_cust_path()
        self.get_cust_files()
        self.get_data_path()

    def get_cust_path(self):
        p = os.getcwd()
        l = len(p)-1
        while p[l] != '/':
            l=l-1
        self.custom_path = p[0:l] + CUST_PATH

    def get_files(self, p, file_list):
        file_list = os.listdir(p)

    def get_cust_files(self):
        self.custom_files = os.listdir(self.custom_path)
        for x in range(len(self.custom_files)):
            newString = ''
            l = 0
            while self.custom_files[x][l] != '.':
                newString = newString + self.custom_files[x][l]
                l = l+1
            self.custom_files[x]= newString

    def get_data_path(self):
        p = os.getcwd()
        l = len(p)-1
        while p[l] != '/':
            l=l-1
        self.data_path = p[0:l] + DATA_PATH
