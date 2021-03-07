import ctypes

data = [list('okfhgierhighiioejkfuhkjgoijljgredr'), list('lkjghoeirgjokfgirehgkherlelhre'), list('hkerjgoewrjgoejfdhiuehrkheherhkekertr')]
SourceData = [[ord(s) for s in d] for d in data]
mxSequences = [Id for IdList in SourceData for Id in IdList]
seqLengths = [len(SourceData[idx])  for idx in range(len(SourceData))]
seqNumber = len(SourceData)
prepr = False

mxSeq_c = (ctypes.c_ushort * len(mxSequences))(*mxSequences)
seqLen_c = (ctypes.c_ulong * len(seqLengths))(*seqLengths)
prepr_c = ctypes.c_bool(False)

clib = ctypes.CDLL("C:\\PRG\\C++\\MLCS\\x64\\Release\\MLCS.dll")
clib.ComputeMCLSs.argtypes = (ctypes.POINTER(ctypes.c_ushort), ctypes.POINTER(ctypes.c_ulong), ctypes.c_ulong, ctypes.c_bool, ctypes.c_ulong)
subseqNum_c = ctypes.c_ulong(0)
subseqLen_c = ctypes.POINTER(ctypes.c_ulong)()
clib.get_subsequences.argtypes = (ctypes.POINTER(ctypes.POINTER(ctypes.c_ulong)), ctypes.POINTER(ctypes.c_ulong))
clib.get_subsequences.restype = ctypes.POINTER(ctypes.c_ushort)

if clib.ComputeMCLSs(mxSeq_c, seqLen_c, seqNumber, prepr_c, 0):
    res = clib.get_subsequences(subseqLen_c, subseqNum_c)
D=subseqLen_c[:subseqNum_c.value]
baseIdx = 0
subSequence = []
for idx in range(subseqNum_c.value):
    subSequence.append(res[baseIdx:baseIdx+D[idx]])
    baseIdx = baseIdx + D[idx]
for i in range(len(subSequence)):
    print(''.join([chr(n) for n in subSequence[i]]))
print(len(subSequence[0]))