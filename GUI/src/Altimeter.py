import os
import shutil

CUST_PATH = "/cust/"
DATA_PATH = "/data/"

class Altimeter:

    def __init__(self):
        self.data_path_default = ""
        self.custom_path = ""
        self.data_path_custom = ""
        self.card_path = ""
        self.custom_extension = ""

        self.custom_files = []
        self.card_files = []
        self.data_files = []
        self.custom_input = []

        self.get_cust_path()
        self.get_cust_files()
        self.get_data_path()


    def set_card_path(self,p):
        if (len(p)==0):
            return
        if (p[len(p)-1]!='/'):
            p=p+'/'
        self.card_path = p

    def get_cust_path(self):
        p = os.getcwd()
        l = len(p)-1
        while p[l] != '/':
            l=l-1
        self.custom_path = p[0:l] + CUST_PATH

    def get_cust_files(self):
        self.custom_files = os.listdir(self.custom_path)
        for x in range(len(self.custom_files)):
            new_string = ''
            l = 0
            while self.custom_files[x][l] != '.':
                new_string = new_string + self.custom_files[x][l]
                l = l+1
            self.custom_files[x]= new_string

    def get_data_path(self):
        p = os.getcwd()
        l = len(p)-1
        while p[l] != '/':
            l=l-1
        self.data_path = p[0:l] + DATA_PATH

    def generate_custom(self):
        out = 0
        if (not os.path.isdir(self.card_path)):
            out = -1

        custom_file = self.custom_path + self.custom_input[0] + ".txt"
        card_file = self.card_path +  "cust.txt"

        f1 =open(custom_file,'w')
        for x in range(1,len(self.custom_input)-1):
            f1.write(self.custom_input[x]+'\n')
        f1.close()
        if (out == 0):
            f2 =open(card_file,'w')
            for x in range(1,len(self.custom_input)-1):
                f2.write(self.custom_input[x]+'\n')
            f2.close()
        self.custom_input = []
        return out

    def upload_existing_custom(self):
        if (not os.path.isdir(self.card_path)):
            return -1
        if (self.custom_extension == ""):
            return -2
        shutil.copyfile(self.custom_path+self.custom_extension,self.card_path+"cust.txt") 
        return 0


