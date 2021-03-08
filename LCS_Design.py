class FindingCommonSubsequence:
    def __init__(self):
        self.alphabet = set()
        self.ncsg = {}
        self.sinkPoint = None
        self.sourcePoint = None
        
    def Preprocessing(self, Sequences):
        if len(Sequences) > 0:
            self.alphabet = set(Sequences[0])
            for i in range(1, len(Sequences)):
                self.alphabet = self.alphabet.intersection(Sequences[i])
        else:
            return()
        res = []
        for i in range(len(Sequences)):
            res.append([s for s in Sequences[i] if s in self.alphabet])
        return(res)

    def SuccessorTable(self, Sequences):
        res = []
        for i in range(len(Sequences)):
            currTable = {char: [] for char in self.alphabet}
            for j in range(len(Sequences[i])):
                for key in currTable.keys():
                    try:
                        pos = Sequences[i][j:].index(key) + j

                    except:
                        continue
                    currTable[key].append(pos+1)
            res.append(currTable)
        return(res) 

    def ConstructedNCSG(self, tables, data):
        self.sourcePoint = tuple([0 for _ in data])
        self.sinkPoint = tuple([65535 for _ in data])
        level_keys = set()
        level_keys.add(self.sourcePoint)
        self.ncsg[self.sourcePoint] = {'char': '', 'out': set(), 'in': set()}
        while len(level_keys) > 0:
            tmp_keys = set()
            for key in level_keys:               
                for char in self.alphabet:
                    link = []
                    for i in range(len(tables)):
                        try:
                            link.append(tables[i][char][key[i]])
                        except:
                            link = self.sinkPoint
                            break
                    link = tuple(link)
                    if (link != self.sinkPoint) and (not link in self.ncsg):
                        tmp_keys.add(link)
                    if not link in self.ncsg:
                        self.ncsg[link] = {'char': char, 'out': set(), 'in': set()}
                    self.ncsg[link]['in'].add(key)
                    self.ncsg[key]['out'].add(link)
            level_keys = tmp_keys

    def ForwardTopSort(self, data):
        level_keys = set()
        level_keys.add(self.sourcePoint)
        layers = []
        while len(level_keys) > 0:
            layers.append(level_keys)
            tmp_keys = set()
            for key in level_keys:
                mustRemoved = set()
                for link in self.ncsg[key]['out']:
                    if len(self.ncsg[link]['in'].difference(level_keys)) > 0:
                        self.ncsg[link]['in'].discard(key)
                        mustRemoved.add(link)
                    else:
                        tmp_keys.add(link)
                self.ncsg[key]['out'].difference_update(mustRemoved)
            level_keys = tmp_keys
        return(layers)            

    def GetMLCS_3(self):
        def dfs_paths(graph, start, goal):
            paths = []
            stack = [(start, [start])]
            while stack:
                (vertex, path) = stack.pop()
                for next in graph[vertex]['in']:
                    if next == goal:
                        paths.append(path[1:])
                    else:
                        stack.append((next, path + [next]))
            return paths
        paths = dfs_paths(self.ncsg, self.sinkPoint, self.sourcePoint)
        sequences = []
        for path in paths:
            sequence = []
            for key in reversed(path):
                 sequence.append(self.ncsg[key]['char'])
            sequences.append(sequence)
        return(sequences)            

lcs = FindingCommonSubsequence()


data = [list('TGACGATC'), list('ATGCTCAG'), list('CTAGTACG')]
dataPreprocessed = lcs.Preprocessing(data)
tables = lcs.SuccessorTable(dataPreprocessed)
lcs.ConstructedNCSG(tables, dataPreprocessed)
layers = lcs.ForwardTopSort(dataPreprocessed)
mlcs = lcs.GetMLCS_3()

lenIndex = [len(mlcs[idx])  for idx in range(len(mlcs))]
_, sortIndex = zip(*sorted([v, idx] for idx, v in enumerate(lenIndex)))
for i in reversed(sortIndex):
    print(''.join(mlcs[i]))
print(len(mlcs[i]))
print(len(mlcs))
