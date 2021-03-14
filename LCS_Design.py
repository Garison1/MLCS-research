class FindingCommonSubsequence:
    def __init__(self):
        self.alphabet = set()
        self.ncsg = {}
        self.sinkPoint = None
        self.sourcePoint = None
        
    def Preprocessing(self, Sequences): #В функцию передается список исходных последовательностей
        if len(Sequences) > 0: #Проверяем, не пуст ли переданный список
            self.alphabet = set(Sequences[0]) #Запоминаем алфавит первой последовательности из списка
            for i in range(1, len(Sequences)): #Перебираем остальные последовательности
                self.alphabet = self.alphabet.intersection(Sequences[i]) #Удаляем уникальные элементы
        else:
            return()
        res = []
        for i in range(len(Sequences)):
            res.append([s for s in Sequences[i] if s in self.alphabet]) #Составляем обновленный список последовательностей
        return(res)

    def SuccessorTable(self, Sequences): #На вход функции передается список последовательностей
        res = []
        for i in range(len(Sequences)): #Рассматриваем каждую последовательность по очереди
            currTable = {char: [] for char in self.alphabet} #Создаем словарь, ключами которого являются элементы алфавита
            for j in range(len(Sequences[i])): #Перебираем элементы текущей последовательности
                for key in currTable.keys(): #Перебираем алфавит
                    try:
                        pos = Sequences[i][j:].index(key) + j #Получаем позицию первого вхождения элемента алфовита key после текущего элемента последовательности j 

                    except:
                        continue #Если таких вхождений нет, переходим к следующему элменту алфавита
                    currTable[key].append(pos+1) #Формируем таблицу, заполняя её индексами первых вхождений текущего элемента алфавита после текущего элемента последовательности j
            res.append(currTable) #В переменную res записываем получившиеся таблицы, которые и возвращает функция
        return(res) 

    def ConstructedNCSG(self, tables, data): #На вход функции передаются таблицы Successor Tables, а так же список отсортированных последовательностей
        self.sourcePoint = tuple([0 for _ in data]) #Формируем нулевую, начальную точку
        self.sinkPoint = tuple([65535 for _ in data]) #Формируем конечную точку
        level_keys = set() #Множество вершин текущего слоя
        level_keys.add(self.sourcePoint) #Первый слой состоит только из начальной точки
        self.ncsg[self.sourcePoint] = {'char': '', 'out': set(), 'in': set()} #Формируем NCSG, к каждой вершине NCSG привязано множество входящих и исходяцих вершин, а так же элемент алфавита
        while len(level_keys) > 0: #Временная переменная с вершинами текущего слоя
            tmp_keys = set() #Временная переменная с вершинами текущего слоя
            for key in level_keys: #Перебираем вершины последнего найденного слоя
                for char in self.alphabet: #Перебираем элементы алфавита
                    link = [] #Временная переменная с координатами текущей вершины
                    for i in range(len(tables)): #Перебираем таблицы
                        try:
                            link.append(tables[i][char][key[i]]) #Для каждой вершины, начиная с исходной (0, 0, 0), в соответствующих её координатам таблицах для каждого элемента алфавита находим координаты новых вершин с которыми данная вершина имеет связь
                        except:
                            link = self.sinkPoint #Если искомой координаты нет, значит мы пришли в конечную точку NCSG
                            break
                    link = tuple(link) #Преобразуем полученные координаты вершины в кортеж
                    if (link != self.sinkPoint) and (not link in self.ncsg):
                        tmp_keys.add(link)
                    if not link in self.ncsg:
                        self.ncsg[link] = {'char': char, 'out': set(), 'in': set()}
                    self.ncsg[link]['in'].add(key) #Добавляем связь, входящую в найденную вершину из предыдущей
                    self.ncsg[key]['out'].add(link) #Добавляем связь, исходящую из предидущйе вершины в найденную
            level_keys = tmp_keys #Определяем список найденных вершин нового слоя

    def ForwardTopSort(self, data): #На вход функции передаются список отсортированных последовательностей
        level_keys = set() #Ключи текущего слоя
        level_keys.add(self.sourcePoint) #Добавляем нулевую точку в качестве первого слоя
        layers = [] #Объявляем список множеств вершин каждого из слоев
        while len(level_keys) > 0:
            layers.append(level_keys) #Записываем готовый слой в переменную
            tmp_keys = set() #Временная переменная с множеством вершин текущего слоя
            for key in level_keys: #Перебераем вершины текущего слоя
                mustRemoved = set() #Множество ключей, с которыми нужно оборвать связи
                for link in self.ncsg[key]['out']: #Перебераем выходы текущей вершины
                    if len(self.ncsg[link]['in'].difference(level_keys)) > 0: #Проверяем, есть ли у вершины связи с вершинами своего слоя
                        self.ncsg[link]['in'].discard(key) #Если такие связи есть, обрываем вход в текущую вершину для вершины предыдущего слоя
                        mustRemoved.add(link) #Записываем в переменную вершину, выход к которой необходимо оборвать
                    else:
                        tmp_keys.add(link) #Если таких связей нет, записываем текущую вершину без изменений
                self.ncsg[key]['out'].difference_update(mustRemoved) #Обрываем выход из текущей вершины к записанной ранее
            level_keys = tmp_keys #Записываем готовые вершины текущего слоя
        return(layers)            

    def GetMLCS_3(self):
        def dfs_paths(graph, start, goal):
            paths = [] #Переменная, в которую мы будем записывать найденные пути
            stack = [(start, [start])] #Переменная, в которую мы временно записываем вершины и пути к ним
            while stack:
                (vertex, path) = stack.pop() #Рассматриваем последнюю вершину из переменной stack и путь к ней
                for next in graph[vertex]['in']: #Перебираем все вершины входящие в данную
                    if next == goal: #Проверяем, является ли вершина входящая в данную начальной точкой (0, 0, 0)
                        paths.append(path[1:]) #Если да, записываем полученный путь в переменную paths
                    else:
                        stack.append((next, path + [next])) #В противном случае записываем входящую вершину и путь к ней в переменную stack
            return paths #Функция возвращает список всех путей из конца NCSG в начало
        paths = dfs_paths(self.ncsg, self.sinkPoint, self.sourcePoint) #Получаем список всех путей из конца NCSG в начало
        sequences = []
        for path in paths: #Перебираем полученные пути
            sequence = []
            for key in reversed(path): #Перебираем вершины текущего пути из начала в конец
                 sequence.append(self.ncsg[key]['char']) #Формируем подпоследовательности начальных последовательностей
            sequences.append(sequence) #Записываем найденную подпоследовательность
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
