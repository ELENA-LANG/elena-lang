namespace text
{
   import system'io;
   
   internal /*const */byte[] base_map = new byte[] 
   {  65, 66, 67, 68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,
      97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,
      48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 43, 47         
   };
   
   public singleton Base64Encoding
   {
      internal int encoding(byte[] src, int src_index, ref int src_len, byte[] dest, int dest_len)
      {
         byte[] map := base_map; // !! temporally
         
         int buffer_index := 0;
         byte buffer[4];
         
         int len := src_len;
         int end := src_index + len;
         int dest_index := 0;
         for (int i := src_index; i < end; i++) {
            buffer[buffer_index] := src[i];
            buffer_index++;
            
            if (buffer_index == 3) {
               dest[dest_index] := map[buffer[0] shr:2]; dest_index++;
               dest[dest_index] := map[((buffer[0] & 3) shl:4) + (buffer[1] shr:4)]; dest_index++;
               dest[dest_index] := map[((buffer[1] & 0Fh) shl:2) + (buffer[2] shr:6)]; dest_index++;
               dest[dest_index] := map[buffer[2] & 3Fh]; dest_index++;
               buffer_index := 0;
               
               if (dest_len - dest_index < 5) {
                  src_len := end - i;
                  
                  :break;
               }
            }
         };
         
         if (buffer_index > 0) {
            dest[dest_index] := map[buffer[0] shr:2]; dest_index++;
            if (buffer_index == 1) {
               dest[dest_index] := map[((buffer[0] & 3) shl:4)]; dest_index++;
               dest[dest_index] := 61; dest_index++;
            }
            else {
               dest[dest_index] := map[((buffer[0] & 3) shl:4) + (buffer[1] shr:4)]; dest_index++;
               dest[dest_index] := map[(buffer[1] & 0Fh) shl:2]; dest_index++;
            };
            dest[dest_index] := 61; dest_index++;
         };
         
         ^ dest_index
      }
      
      internal int decoding(byte[] src, int src_index, ref int src_len, byte[] dest, int dest_len)
      {
         byte[] map := base_map; // !! temporally
         
         byte buffer_index := 0;
         byte buffer[4];
         
         int len := src_len;
         int end := src_index + len;
         int dest_index := 0;
         for (int i := src_index; i < end; i++) {
            int map_index := 0;
            while (map_index < 64) {
               if (map[map_index] == src[i]) {
                  :break;
               };
               
               map_index++;
            };
            
            buffer[buffer_index] := map_index; buffer_index++;
            if (buffer_index == 4) {
               dest[dest_index] := (buffer[0] shl:2) + (buffer[1] shr:4); dest_index++;
               if (buffer[2] != 64) {
                  dest[dest_index] := (buffer[1] shl:4) + (buffer[2] shr:2); dest_index++;
               };
               if (buffer[3] != 64) {
                  dest[dest_index] := (buffer[2] shl:6) + buffer[3]; dest_index++;
               };
               buffer_index := 0;
                              
               if (dest_len - dest_index < 5) {
                  src_len := end - i;
                  
                  :break;
               }               
            }            
         };
         
         ^ dest_index;
      }
      
      string toString(int index, int size, byte[] buffer)
      {
         auto builder := new StringBuilder();
         byte tmp[1024];
         int tmp_len := 1024;
         int len := size;
         int src_index := index;
         while (len > 0) {
            if (len < 1024) {
               tmp_len := len;
            }
            else tmp_len := 1024;
            
            int dest_len := encoding(buffer, src_index, tmp_len, tmp, 1024);
            builder.write(tmp, 0, dest_len);
            len -= tmp_len;
            src_index += tmp_len;
         };
         
         ^ *builder;
      }
      
      int fromString(string s, MemoryBuffer output)
      {
         int saved := 0;
         int len := s.Length;
         
         byte dest_tmp[1024];
         byte tmp[1024];
         int tmp_len := 1024;
         int src_index := 0;
         while (len > 0) {
            if (len < 1024) {
               tmp_len := len;
            }
            else tmp_len := 1024;

            s.saveToByteArray(src_index, tmp, tmp_len);
            
            int dest_len := decoding(tmp, 0, tmp_len, dest_tmp, 1024);
            output.write(0, dest_len, dest_tmp);
            saved += dest_len;
            
            src_index += tmp_len;
            len -= tmp_len;
         };
         
         ^ saved;
      }
      
      byte[] fromString(string s)
      {
         auto output := MemoryBuffer.allocate(s.Length);

         fromString(s, output);
         
         ^ *output
      }
   }
   
   public sealed singleton Base64Encoder : Encoder
   {
      int convertFrom(byte[] byteArray, int index, ref int len, char[] output, int outputIndex, int outputLen)
      {
         auto dest_output := MemoryBuffer.allocate(len);
         
         byte tmp[1024];
         int tmp_len := 1024;
            
         int src_len := len;
         int src_index := index;
         while (src_len > 0) {
            if (src_len < 1024) {
               tmp_len := src_len;
            }
            else tmp_len := 1024;
            
            int dest_len := Base64Encoding.encoding(byteArray, src_index, tmp_len, tmp, 1024);
            dest_output.write(0, dest_len, tmp);
            
            len -= src_len;
            src_index += src_len;
         };
         
         int total_len := dest_output.Length;
         
         ^ UTF8Encoder.convertFrom(*dest_output, 0, ref total_len, output, outputIndex, outputLen);
      }

      int convertTo(char[] chars, int index, ref int len, byte[] output, int outputIndex, int outputLen)
      {
         auto temp_buffer := MemoryBuffer.allocate(outputLen);
         
         int maximalSize := outputLen;
         
         byte tmp[1024];
         int tmp_len := 1024;
         int src_len := len;
         int src_index := 0;
         while (src_len > 0) {
            if (maximalSize < 1024) {
               tmp_len := maximalSize;
            }
            else tmp_len := 1024;            
            
            int sub_len := src_len;
            int byte_len := UTF8Encoder.convertTo(chars, src_index, ref sub_len, tmp, 0, tmp_len);
            
            maximalSize -= byte_len;
            
            temp_buffer.write(0, byte_len, tmp);
            
            src_index += sub_len;
            src_len -= sub_len;
         };
         
         int temp_buffer_len := temp_buffer.Length;
         int convered := Base64Encoding.decoding(*temp_buffer, 0, ref temp_buffer_len, tmp, 1024);
         
         Array<byte>.copyTo(output, *temp_buffer, outputIndex, temp_buffer_len);
         
         ^ temp_buffer_len;
      }      
      
      int convertChar(char ch, byte[] byteArray, int index)
      {
         NotSupportedException.raise();
         
         ^ 0
      }

      char convertToChar(byte[] byteArray, int index, ref int sourLen)
      {
         NotSupportedException.raise();
         
         ^ 0
      }   
   }
}