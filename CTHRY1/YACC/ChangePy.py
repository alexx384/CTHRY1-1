
def replaceUnion(path):
    
    In = open(path, "r")
    text = str(In.read())
    In.close()

    text = text.replace('union', 'struct')

    Out = open(path, "w")
    Out.write(text)
    Out.close()


if __name__ == "__main__":
    
    f = open('parse.cpp.h', 'r')
    s = str(f.read())
    f.close()

    f = open('parse.cpp.h', 'w')
    
    if (False == s.startswith('#include "../VarStor.h"\n')):
        s = '#include "../VarStor.h"\n' + s    

    f.write(s)
    f.close()

    replaceUnion('parse.cpp')
    replaceUnion('parse.cpp.h')






