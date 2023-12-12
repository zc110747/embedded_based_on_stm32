
import os
import shutil
import glob

deletedirs=[ "project\\DebugConfig", "project\\Listings", "project\\Objects"]
deletefiledir="project"

def list_directorise(path):
    print(path)
    return [os.path.join(path, dir) for dir in os.listdir(path) if os.path.isdir(os.path.join(path, dir))]

directories = list_directorise(os.path.dirname(os.path.abspath(__file__)))

file_extension = ".{0}".format(os.getlogin())

for directory in directories:
    #deletefile
    for deletefile in glob.glob(os.path.join(directory, deletefiledir, f'*{file_extension}')):
        os.remove(deletefile)
        print("delete file:{0}".format(deletefile))

    #delete folder
    for deletedir in deletedirs:
        deletefolder=os.path.join(directory, deletedir)
        if os.path.exists(deletefolder):
            shutil.rmtree(deletefolder)
            print("delete folder:{0}".format(deletefolder))

print("success run delete, finished!")
input("Press Enter to exit...")