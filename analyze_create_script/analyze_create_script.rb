#! /usr/bin/env ruby
# coding: utf-8

require 'csv'
class Object
  def is_number?
    to_f.to_s == to_s || to_i.to_s == to_s
  end
end

class AnalyzeScript
  def initialize(path)
    @script_file = File.open(path, 'r')
    @tables = []
  end

  def run
    read_create_table
#    @tables[0].dump_columns
    @tables.each do |t|
      t.dump_columns
    end
  end

  def read_create_table
    table = nil
    add_line = false    
    @script_file.each do |line|
      line.chomp!
      if line =~ /CREATE TABLE/
        table_name = parse_table_name(line)
        table = Table.new(table_name)
        @tables.push table
        add_line = true
        next
      end

      if add_line
        table.parse_line(line)
      end

      if line =~ /;/
        add_line = false
      end
    end
  end

  def self.raw_name(org_name)
    org_name.gsub(/`/, "")
  end
  
  def parse_table_name(line)
    elems = line.split(/\s+/)
    table_name = nil
    elems.each do |elem|
      if ["CREATE", "TABLE", "IF", "NOT", "EXISTS", "("].include?(elem)
        next
      end
      table_name = AnalyzeScript.raw_name(elem)
      break
    end
    table_name
  end
  
  class Table
    attr_accessor :table_name, :columns, :mode
    COLUMN = 0
    CONSTRAINT = 1
    OPTION = 2
    def initialize(table_name)
      @table_name = table_name
      @columns = []
      @mode = COLUMN
    end
    
    def parse_line(line)
      line.strip!
      elems = line.split(/[\s+|\(|\)|,]/)
      elems = elems.select{|e| e != ""}
      if ["KEY", "INDEX", "CONSTRAINT", "UNIQUE"].first.include?(elems.first)
       # DB constraint
        @mode = CONSTRAINT
      elsif ["ENGINE", "AUTO_INCREMENT", "DEFAULT"].include?(elems.first)
        # DB option
        @mode = OPTION
      elsif @mode == COLUMN
        column = Column.new(elems)
        return if column.column_name.nil? or column.column_name == ""
        @columns.push column
      end
    end
    
    def dump_columns
      CSV.open("#{@table_name}.tsv", "w", col_sep: "\t", force_quotes: true) do |csv|
        @columns.each do |c|
          csv << [c.column_name,
                  c.main_type,
                  c.type_length,
                  c.type_option,
                  c.null_ok.nil? ? "○" : "",
                  c.option,
                  c.default_val,
                  c.comment_val,
                 ]
        end
      end
    end
  end

  class Column
    attr_accessor :column_name,
                  :main_type,
                  :type_length,
                  :type_option,
                  :null_ok,
                  :default_val,
                  :option,
                  :comment_val,
                  :pk
    def initialize(elems)
      if elems.first.upcase == "PRIMARY"
        @pk = parse_pk(elems.drop(1))
      else
        @column_name = AnalyzeScript.raw_name(elems.first)
        return if @column_name.nil? or @column_name == ""
        parse(elems.drop(1))
      end
    end

    def parse(attrs)
      pos = 0
      @main_type = attrs[pos] # type
      pos += 1

      return if attrs.length < pos + 1
      if attrs[pos].is_number?
        @type_length = attrs[pos]
        pos += 1
      end

      return if attrs.length < pos + 1      
      if ["UNSIGNED", "BINARY", "COLLATE"].include?(attrs[pos])
        if attrs[pos] == "COLLATE"
          @type_option = attrs[pos] + " " + attrs[pos + 1]
          pos += 2
        else
          @type_option = attrs[pos]
          pos += 1          
        end
      end

      return if attrs.length < pos + 1      
      if attrs[pos] == "NOT"
        attrs[pos + 1]  == "NULL"
        @null_ok = nil
        pos += 2
      end

      return if attrs.length < pos + 1      
      if attrs[pos] == "NULL"
        @null_ok = true
        pos += 1
      end

      return if attrs.length < pos + 1
      if attrs[pos] == "DEFAULT"
        @default_val = attrs[pos + 1]
        pos += 2
      end
      
      return if attrs.length < pos + 1
      if attrs[pos] != "COMMENT"
        @option = attrs[pos]
        pos += 1
      end
      
      return if attrs.length < pos + 1
      p "COMMENT?"
      p attrs[pos]
      if attrs[pos] == "COMMENT"
        @comment_val = attrs[pos + 1]
        
        pos += 2
      end
      

    end

    def parse_pk(attrs)
      pk = nil
      attrs.each do |attr|
        next if attr == "KEY"
        pk = AnalyzeScript.raw_name(attr)
        break
      end
      pk
    end
  end

  class Constrain
  end
end



filepath = ARGV[0]
if filepath.nil?
  warn "argument missing"
  exit
end

unless File.exist? filepath
  warn "create script file not found"
  exit
end

AnalyzeScript.new(filepath).run

