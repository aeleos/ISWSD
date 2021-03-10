import os
import shutil
import csv
from datetime import datetime,timedelta

CUST_FOLDER = "/preset/"
DATA_FOLDER = "/data/"
READ_INVALID_DATE = 0
WRITE_INVALID_DATE = "NaN"
READ_INVALID_TIME = 0xFFFFFFFF
WRITE_INVALID_TIME = "NaN"
READ_INVALID_COORDINATE = 0
WRITE_INVALID_COORDINATE = "NaN"
PRESET_ZERO_TAG = 'P'
FIELD_ZERO_TAG = 'F'
MEASUREMENT_TAG = 'M'
PRESET_FILE = "preset.txt"
DATA_FILE = "data.csv"
DATA_WRITE_HEADER = "Measurement,Date,Time,Latitude,Longitude,Height(m)\n"

class Altimeter:

    def __init__(self):
        self.data_path_default = ""        # path to the data folder in the GUI folder
        self.preset_path = ""              # path to the preset folder in the GUI folder
        self.data_path = ""                # path to the data (either default or user input) 
        self.card_path = ""                # path the user input to the SD card
        self.preset_file_name = ""         # if user choses to upload an existing preset, then the file name

        self.preset_files = []             # existing files in the /GUI/cust/ folder
        self.card_presets = []             # points read from PRESET_FILE on the SD card
        self.data_files = []               # files in the data folder MAYBE DELETE
        self.preset_input = []             # user input from the form tab 2 while generating preset  

        self.get_paths()
        self.get_cust_files()


    def set_card_path(self,p):
        if (len(p)==0):
            return
        if (p[len(p)-1]!='/'):
            p=p+'/'
        self.card_path = p

    def set_data_path(self,p):
        if (len(p)==0):
            return
        if (p[len(p)-1]!='/'):
            p=p+'/'
        self.data_path = p

    def get_paths(self):
        p = os.getcwd()
        l = len(p)-1
        while p[l] != '/':
            l=l-1
        self.preset_path = p[0:l] + CUST_FOLDER
        self.data_path_default = p[0:l] + DATA_FOLDER

    def get_cust_files(self):
        self.preset_files = os.listdir(self.preset_path)
        for x in range(len(self.preset_files)):
            new_string = ''
            l = 0
            while self.preset_files[x][l] != '.':
                new_string = new_string + self.preset_files[x][l]
                l = l+1
            self.preset_files[x]= new_string

    def generate_preset(self):
        out = 0
        if (not os.path.isdir(self.card_path)):
            out = -1

        preset_file = self.preset_path + self.preset_input[0] + ".txt"
        card_file = self.card_path +  PRESET_FILE

        f1 = open(preset_file,'w')
        for x in range(1,len(self.preset_input)-1):
            f1.write(self.preset_input[x]+'\n')
        f1.write(self.preset_input[len(self.preset_input)-1])
        f1.close()
        if (out == 0):
            f2 =open(card_file,'w')
            for x in range(1,len(self.preset_input)-1):
                f2.write(self.preset_input[x]+'\n')
            f2.write(self.preset_input[len(self.preset_input)-1])
            f2.close()
        self.preset_input = []
        return out

    def upload_existing_preset(self):
        if (not os.path.isdir(self.card_path)):
            return -1
        if (self.preset_file_name == ""):
            return -2
        shutil.copyfile(self.preset_path+self.preset_file_name,self.card_path + PRESET_FILE) 
        return 0

    def offload_data(self):
        if (not os.path.isdir(self.card_path)):
            return -1
        if (not os.path.isdir(self.data_path)):
            print(self.data_path)
            return -2

        if (os.path.isfile(self.card_path + PRESET_FILE)):
            fp = open(self.card_path + PRESET_FILE,'r')
            for line in fp:
                self.card_presets.append(line)
            fp.close()
            os.remove(self.card_path + PRESET_FILE)
        
        data_read_file = csv.reader(open(self.card_path + DATA_FILE,'r'))
        data_write_file = False

        offload_datetime = datetime.now
        measurement_counter = 0
        preset_chosen = 0

        for row in data_read_file:

            zero_code = row[0]

            if (zero_code == PRESET_ZERO_TAG) or (zero_code == FIELD_ZERO_TAG):

                if (zero_code == PRESET_ZERO_TAG):
                    preset_chosen = 1
                else:
                    preset_chosen = 0

                if (data_write_file):
                    data_write_file.close()
                
                if (row[1] != READ_INVALID_DATE) and (row[2] != READ_INVALID_TIME):
                    data_file_name = zero_code+row[1]+row[2]
                else:
                    data_file_name = zero_code + offload_datetime.strftime("%m%d%Y%H%M%S")
                    offload_datetime = offload_datetime + timedelta(0,1)

                measurement_counter = 0
                    
                data_write_file = open(self.data_path + data_file_name + ".csv",'w')
                data_write_file.write(DATA_WRITE_HEADER)

            # Process Measurement   

            if (preset_chosen):
                data_write_measurement = ''
                for letter in self.card_presets[measurement_counter]:
                    if letter != '\n':
                        data_write_measurement = data_write_measurement + letter
            else:
                data_write_measurement = str(measurement_counter)
            data_write_file.write(data_write_measurement + ',')
            measurement_counter = measurement_counter + 1

            # Process Date    

            if (row[1] == READ_INVALID_DATE):
                data_write_date = WRITE_INVALID_DATE
            else:
                data_write_date = row[1][0]+row[1][1]+'/'+row[1][2]+row[1][3]+'/'+row[1][4]+row[1][5]
            data_write_file.write(data_write_date + ',')

            # Process Time

            if (row[2] == READ_INVALID_TIME):
                data_write_time = WRITE_INVALID_TIME
            else:
                data_write_time = row[2][0]+row[2][1]+':'+row[2][2]+row[2][3]+':'+row[2][4]+row[2][5]
            data_write_file.write(data_write_time + ',')

            # Process Latitude

            if (row[3] == READ_INVALID_COORDINATE):
                data_write_latidtude = WRITE_INVALID_COORDINATE
            else:
                data_write_latitude = row[3]
            data_write_file.write(data_write_latitude + ',')

            # Process Longitude

            if (row[4] == READ_INVALID_COORDINATE):
                data_write_longitude = WRITE_INVALID_COORDINATE
            else:
                data_write_longitude = row[4]
            data_write_file.write(data_write_longitude + ',')

            # Process height

            data_write_height = row[5]
            data_write_file.write(data_write_height + '\n')


        data_write_file.close()

        os.remove(self.card_path + DATA_FILE)

        self.card_presets = []

        return 0



