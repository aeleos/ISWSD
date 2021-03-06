import os

class Altimeter:

    def __init__(self):
        self.data_path = ""
        self.custom_path = ""
        self.card_path = ""

        self.custom_files = []
        self.card_files = []
        self.data_files = []

        self.get_cust_path()
        self.get_cust_files()

    def get_cust_path(self):
        p = os.getcwd()
        l = len(p)-1
        while p[l] != '/':
            l=l-1
        self.custom_path = p[0:l] + "/cust/"

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
