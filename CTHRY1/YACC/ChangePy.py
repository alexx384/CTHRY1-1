from shutil import copyfile
import os

def replaceUnion(path):
    
    In = open(path, "r")
    text = str(In.read())
    In.close()

    text = text.replace('union', 'struct')

    Out = open(path, "w")
    Out.write(text)
    Out.close()


include_str = '#include "VarStor.h"\n'
cpp_file = 'parse.cpp'
cpp_file_h = 'parse.cpp.h'

    
f = open(cpp_file_h, 'r')
s = str(f.read())
f.close()

f = open(cpp_file_h, 'w')
    
if (False == s.startswith(include_str)):
    s = include_str + s    

f.write(s)
f.close()

replaceUnion(cpp_file)
replaceUnion(cpp_file_h)
	
upper_dir = os.path.dirname(os.getcwd())
	
copyfile(cpp_file, upper_dir + '\\' + cpp_file)
copyfile(cpp_file_h, upper_dir + '\\' + cpp_file_h)

os.remove(cpp_file)
os.remove(cpp_file_h)

