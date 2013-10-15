//#задать basic'* = std'basic'*.
//#задать list'*  = std'collections'*.
//#задать ctrl'*  = std'patterns'*.
//
//#задать text'*  = ext'text'*.
//#задать io'*   = ext'io'*.
//
//// --- WordList ---
//
//#класс WordList
//{
//    #поле Список.
//    #поле Всего.
//
//    #метод new
//    [
//        Список := list'List.
//        Всего := basic'Integer << 0.
//    ]
//
//    #метод += Слово
//    [
//        Всего += 1.
//        
//        #если(ctrl'Searching::Список == Слово)!
//            [ Список += basic'LiteralType of:Слово. ].
//    ]
//
//    #метод $датьВсего = Всего.
//
//    #метод $датьУникальных = Список array'Count.
//
//    #дополнение (Список).
//}
//
//// --- Program ---
//
//#символ программа =>
//[
//    'program'output << "Введите текст(чтобы закончить дважды нажмите ENTER):%n".
//
//    #пер Текст := basic'String.
//    #пер Флаг := basic'Boolean << basic'True.
//
//    #пер aReader := #дополнение(Текст)
//    {
//        += Значение
//        [
//            #var Строка := basic'LiteralType of:Значение.
//
//            #if (Строка array'Count != 0)?
//                [ Текст += Значение. ]
//                | [ Флаг << basic'False. ].
//        ]
//    }.
//
//    #цикл Флаг?
//    [    
//        'program'Input >> aReader.
//
//        Текст += " ".
//    ].
//
//    #пер Список := WordList.
//    
//    #группа(ctrl'EEnumeration, text'EWords, Текст) 
//        run: aWord =>(Список += aWord).
//
//    'program'output << "Всего " << Список $датьУникальных << " уникальных слов из " << Список $датьВсего.
//    'program'output << "%nСписок уникальных слов:%n".
//
//    'program'output << #группа(io'EListPrinting, Список).
//
//    'program'input char'get. // wait for any key
//].
