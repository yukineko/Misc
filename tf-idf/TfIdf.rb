# coding: utf-8
require 'mecab'

class TfIdf
  def initialize
    @tagger = MeCab::Tagger.new
    @words_map = Hash.new
    @docs = 0
  end

  def readDir(dir)
    Dir.foreach(dir){|f|
      p dir + f
      next if f == "." or f == ".."
      readDocs (dir + f)
    }
  end
  
  def readDocs(path)
    File.open(path) do |file|
      text = ''
      file.each_line do |line|
        if line.start_with? "<doc" then
          text = ''
        elsif line.start_with? "</doc>" then
          readDoc(text)
          @docs = @docs + 1
        else
          line.chomp!
          text << line
        end
      end
    end
  end

  def readDoc(text)
    result = @tagger.parse(text).split("\n")
    words = Array.new
    result.each {|line|
      if line == "EOS"
        break
      end
      
      elems = line.split("\t")
      word = elems[0]
      word_class = elems[1].split(",")
      if(word.ascii_only?)
        next
      end

      if word_class[0] != "名詞" or !(word_class[1] == "一般" or word_class[1] == '固有名詞')
        next
      end
      words.push word

    }
    words.uniq!
    words.each {|w|
      if (@words_map.key? w) then
        @words_map[w] = @words_map[w] + 1
      else
        @words_map[w] = 1
      end
    }
  end

  def tfidf(text)

  end

  def result
    @words_map
  end

  def doc_num
    @docs
  end
  
end
sentence = '辞典等には以上のようにあるわけだが、これは大きく二分すると「自然言語」と「形式言語」とがあるうちの自然言語について述べている。しかし、1950年代以降の言語学などで
は、定義中にも「記号体系」といった表現もあるように形式的な面やその扱い、言い換え
ると形式言語的な面も扱うようになっており、こんにちの言語学において形式体系と全く
無関係な分野はそう多くはない。形式的な議論では、「その言語における文字の、その言
語の文法に従った並び」の集合が「言語」である、といったように定義される。なお、ジャック・デリダという、フランスの一般にポスト構造主義の代表的哲学者と位置
づけられている哲学者は、「声」を基礎とし文字をその代替とする発想（「音声中心主義
」、"Phonocentrism" とデリダは称するもの）が言語学に存在する、と主張し、それに対
する批判を投げかける立場を主張した（『声と現象』）。『グラマトロジーについて』と
「差延」の記事も参照。'
tfidf = TfIdf.new
root = "/home/yuki/src/ili_tools/tf-idf/docs/"
Dir.foreach(root) do |f|
  next if f == "." or f == ".."
  p root + f
  tfidf.readDir(root + f + "/")
end

r = tfidf.result
File.open("result.txt", "w") do |f|
  r.each {|key, val|
    f.printf("%s\t%s\n", key, val)
  }
  f.puts(tfidf.doc_num)
end
