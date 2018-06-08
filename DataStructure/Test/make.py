import sys
import os
import re
import time
matchCpp = re.compile(r".*\.cpp")
matchExe = re.compile(r".*\.exe")
matchTxt = re.compile(r".*\.txt")

def toCompileCommand(file):
    return "g++ -std=c++17 -O2 " + file  + " -o " + file[:-3]+"exe"
def main():
    command = ""
    if len(sys.argv) == 2:
        command = sys.argv[1]
    else:
        command = "build"
    if command == "build":
        if (len(sys.argv) >= 3):
            shellCommand = toCompileCommand(sys.argv[2])
            print(shellCommand)
            os.system(shellCommand)    
        else:
            for file in list(os.walk("."))[0][2]:
                if matchCpp.match(file):
                    shellCommand = toCompileCommand(file)
                    print(shellCommand)
                    os.system(shellCommand)
                    time.sleep(2)
    elif command == "clean":
        for file in list(os.walk("."))[0][2]:
            if matchExe.match(file) or matchTxt.match(file):
                os.remove(file)
    else:
        print("invalid argument ")+command


if __name__ == '__main__':
    main()
