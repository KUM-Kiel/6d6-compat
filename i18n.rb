class I18n
  def initialize
    @keys = []
    @langs = []
    @strings = {}
  end

  def add_lang name, file
    File.open(file) do |f|
      strings = {}
      k = nil
      v = ''
      c = false
      f.each_line do |l|
        if m = l.match(/^###(#?)([a-z][0-9a-z_]*)$/)
          v.chomp! if c
          strings[k] = v if k
          c = m[1] == '#'
          k = m[2]
          v = ''
        else
          v << l
        end
      end
      strings[k] = v if k

      @keys = (@keys | strings.each_key.to_a).sort
      @langs << name
      @strings[name] = strings
    end
  end

  def write_h file
    raise('No language files added!') unless @langs.length > 0
    File.open(file, 'w') do |f|
      f.puts '// This file was automatically generated.'
      f.puts '// Do not edit!'
      f.puts '#ifndef I18N_HEADER'
      f.puts '#define I18N_HEADER'
      f.puts 'typedef struct {'
      @keys.each do |k|
        f.puts "  const char *#{k};"
      end
      f.puts '} I18n;'
      @langs.each do |l|
        f.puts "extern I18n *#{l};"
      end
      f.puts 'extern I18n *i18n;'
      f.puts "static inline void i18n_set_lang(const char *lang)"
      f.puts "{"
      f.puts "  if (!lang) {"
      f.puts "    i18n = #{@langs.first};"
      @langs.each do |l|
        c = l \
          .each_char \
          .each_with_index.to_a \
          .map{|c|
            upper = c[0].upcase
            lower = c[0].downcase
            if upper == lower
              "lang[#{c[1]}] == '#{c[0]}'"
            else
              "(lang[#{c[1]}] == '#{upper}' || lang[#{c[1]}] == '#{lower}')"
            end
          } \
          .join(' && ')
        f.puts "  } else if (#{c}) {"
        f.puts "    i18n = #{l};"
      end
      @langs.each do |l|
        c = l \
          .each_char \
          .take_while{|c| c != '_'} \
          .each_with_index.to_a \
          .map{|c|
            upper = c[0].upcase
            lower = c[0].downcase
            if upper == lower
              "lang[#{c[1]}] == '#{c[0]}'"
            else
              "(lang[#{c[1]}] == '#{upper}' || lang[#{c[1]}] == '#{lower}')"
            end
          } \
          .join(' && ')
        f.puts "  } else if (#{c}) {"
        f.puts "    i18n = #{l};"
      end
      f.puts "  } else {"
      f.puts "    i18n = #{@langs.first};"
      f.puts "  }"
      f.puts "}"
      f.puts '#endif'
      f.puts
      f.puts '#ifdef I18N_IMPLEMENTATION'
      f.puts '#undef I18N_IMPLEMENTATION'
      @langs.each do |lang|
        f.puts "static I18n _#{lang} = {"
        @keys.each do |k|
          v = @strings[lang][k]
          if !v
            v = @langs.each.lazy.map{|l| @strings[l][k]}.select{|x| x}.first
          end
          f.puts "  .#{k} = #{v.inspect},"
        end
        f.puts '};'
        f.puts "I18n *#{lang} = &_#{lang};"
      end
      f.puts "I18n *i18n = &_#{@langs.first};"
      f.puts '#endif'
    end
  end
end
