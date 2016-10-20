functions = []

U = '((uint8_t *) x)'
I = '((int8_t *) x)'

[:be, :le].each do |byte_order|
  [:int, :uint].each do |sign|
    [8, 16, 32, 64].each do |size|
      type = "#{sign}#{size}_t"
      bytes = (size / 8).times.to_a
      bytes.reverse! if byte_order == :le
      bytes = bytes.each_with_index.map{|b, i|
        shift = size - 8 * (i + 1)
        "((#{type}) #{sign == :int && i == 0 ? I : U}[#{b}]#{shift > 0 ? " >> #{shift}" : ''})"
      }
      functions << "static inline #{type} ld_#{sign[0]}#{size}_#{byte_order}(const void *x){return #{bytes.join(" | ")};}"
    end
  end
end

puts <<-C
#ifndef NUMBER_H
#define NUMBER_H

#include <stdint.h>

#{functions.join "\n"}

#endif
C
