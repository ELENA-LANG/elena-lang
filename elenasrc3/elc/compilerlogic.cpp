//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler logic class implementation.
//
//                                             (C)2021-2024, by Aleksey Rakov
//                                             (C)2024, by ELENA-LANG Org
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "langcommon.h"
#include "compilerlogic.h"
#include "bytecode.h"

using namespace elena_lang;

inline MethodHint operator | (const MethodHint& l, const MethodHint& r)
{
   return (MethodHint)((unsigned int)l | (unsigned int)r);
}

inline MethodHint operator & (const MethodHint& l, const MethodHint& r)
{
   return (MethodHint)((unsigned int)l & (unsigned int)r);
}

inline MethodHint operator & (const ref_t& l, const MethodHint& r)
{
   return (MethodHint)(l & (unsigned int)r);
}

bool testMethodHint(MethodHint hint, MethodHint mask)
{
   return test((ref_t)hint, (ref_t)mask);
}

bool testMethodHint(ref_t hint, MethodHint mask)
{
   return test(hint, (ref_t)mask);
}

typedef CompilerLogic::Op Op;

constexpr auto OperationLength = 207;
constexpr Op Operations[OperationLength] =
{
   {
      EQUAL_OPERATOR_ID, BuildKey::NilCondOp, V_NIL, V_OBJECT, 0, V_FLAG
   },
   {
      EQUAL_OPERATOR_ID, BuildKey::NilCondOp, V_OBJECT, V_NIL, 0, V_FLAG
   },
   {
      NOTEQUAL_OPERATOR_ID, BuildKey::NilCondOp, V_NIL, V_OBJECT, 0, V_FLAG
   },
   {
      NOTEQUAL_OPERATOR_ID, BuildKey::NilCondOp, V_OBJECT, V_NIL, 0, V_FLAG
   },
   {
      SET_INDEXER_OPERATOR_ID, BuildKey::DictionaryOp, V_DICTIONARY, V_INT32, V_STRING, V_OBJECT
   },
   {
      SET_INDEXER_OPERATOR_ID, BuildKey::DictionaryOp, V_DICTIONARY, V_STRING, V_STRING, V_OBJECT
   },
   {
      SET_INDEXER_OPERATOR_ID, BuildKey::DictionaryOp, V_DICTIONARY, V_OBJECT, V_STRING, V_OBJECT
   },
   {
      SET_INDEXER_OPERATOR_ID, BuildKey::DictionaryOp, V_DICTIONARY, V_DECLARATION, V_STRING, V_OBJECT
   },
   {
      NAME_OPERATOR_ID, BuildKey::DeclOp, V_DECLARATION, 0, 0, V_STRING
   },
   {
      NAME_OPERATOR_ID, BuildKey::DeclOp, V_GETTER, 0, 0, V_STRING
   },
   {
      REFERENCE_OPERATOR_ID, BuildKey::DeclOp, V_DECLARATION, 0, 0, V_STRING
   },
   {
      ADD_ASSIGN_OPERATOR_ID, BuildKey::ObjArrayOp, V_OBJARRAY, V_OBJECT, 0, V_OBJECT
   },
   {
      ADD_ASSIGN_OPERATOR_ID, BuildKey::ObjArrayOp, V_OBJARRAY, V_DECLARATION, 0, V_OBJECT
   },
   {
      ADD_OPERATOR_ID, BuildKey::IntOp, V_INT32, V_INT32, 0, V_INT32
   },
   {
      SUB_OPERATOR_ID, BuildKey::IntOp, V_INT32, V_INT32, 0, V_INT32
   },
   {
      MUL_OPERATOR_ID, BuildKey::IntOp, V_INT32, V_INT32, 0, V_INT32
   },
   {
      DIV_OPERATOR_ID, BuildKey::IntOp, V_INT32, V_INT32, 0, V_INT32
   },
   {
      ADD_ASSIGN_OPERATOR_ID, BuildKey::IntOp, V_INT32, V_INT32, 0, 0
   },
   {
      SUB_ASSIGN_OPERATOR_ID, BuildKey::IntOp, V_INT32, V_INT32, 0, 0
   },
   {
      MUL_ASSIGN_OPERATOR_ID, BuildKey::IntOp, V_INT32, V_INT32, 0, 0
   },
   {
      DIV_ASSIGN_OPERATOR_ID, BuildKey::IntOp, V_INT32, V_INT32, 0, 0
   },
   {
      BAND_OPERATOR_ID, BuildKey::IntOp, V_INT32, V_INT32, 0, V_INT32
   },
   {
      BOR_OPERATOR_ID, BuildKey::IntOp, V_INT32, V_INT32, 0, V_INT32
   },
   {
      BXOR_OPERATOR_ID, BuildKey::IntOp, V_INT32, V_INT32, 0, V_INT32
   },
   {
      BNOT_OPERATOR_ID, BuildKey::IntSOp, V_INT32, 0, 0, V_INT32
   },
   {
      NEGATE_OPERATOR_ID, BuildKey::IntSOp, V_INT32, 0, 0, V_INT32
   },
   {
      INC_OPERATOR_ID, BuildKey::IntSOp, V_INT32, 0, 0, V_INT32
   },
   {
      DEC_OPERATOR_ID, BuildKey::IntSOp, V_INT32, 0, 0, V_INT32
   },
   {
      SHL_OPERATOR_ID, BuildKey::IntOp, V_INT32, V_INT32, 0, V_INT32
   },
   {
      SHR_OPERATOR_ID, BuildKey::IntOp, V_INT32, V_INT32, 0, V_INT32
   },
   {
      EQUAL_OPERATOR_ID, BuildKey::IntCondOp, V_INT32, V_INT32, 0, V_FLAG
   },
   {
      LESS_OPERATOR_ID, BuildKey::IntCondOp, V_INT32, V_INT32, 0, V_FLAG
   },
   {
      NOTEQUAL_OPERATOR_ID, BuildKey::IntCondOp, V_INT32, V_INT32, 0, V_FLAG
   },
   {
      GREATER_OPERATOR_ID, BuildKey::IntCondOp, V_INT32, V_INT32, 0, V_FLAG
   },
   {
      EQUAL_OPERATOR_ID, BuildKey::IntCondOp, V_WORD32, V_WORD32, 0, V_FLAG
   },
   {
      LESS_OPERATOR_ID, BuildKey::IntCondOp, V_WORD32, V_WORD32, 0, V_FLAG
   },
   {
      NOTGREATER_OPERATOR_ID, BuildKey::IntCondOp, V_WORD32, V_WORD32, 0, V_FLAG
   },
   {
      NOTLESS_OPERATOR_ID, BuildKey::IntCondOp, V_WORD32, V_WORD32, 0, V_FLAG
   },
   {
      NOTEQUAL_OPERATOR_ID, BuildKey::IntCondOp, V_WORD32, V_WORD32, 0, V_FLAG
   },
   {
      NOTLESS_OPERATOR_ID, BuildKey::IntCondOp, V_PTR32, V_PTR32, 0, V_FLAG
   },
   {
      NOTEQUAL_OPERATOR_ID, BuildKey::IntCondOp, V_PTR32, V_PTR32, 0, V_FLAG
   },
   {
      ADD_OPERATOR_ID, BuildKey::UIntOp, V_UINT32, V_UINT32, 0, V_UINT32
   },
   {
      SUB_OPERATOR_ID, BuildKey::UIntOp, V_UINT32, V_UINT32, 0, V_UINT32
   },
   {
      MUL_OPERATOR_ID, BuildKey::UIntOp, V_UINT32, V_UINT32, 0, V_UINT32
   },
   {
      DIV_OPERATOR_ID, BuildKey::UIntOp, V_UINT32, V_UINT32, 0, V_UINT32
   },
   {
      ADD_ASSIGN_OPERATOR_ID, BuildKey::UIntOp, V_UINT32, V_UINT32, 0, 0
   },
   {
      SUB_ASSIGN_OPERATOR_ID, BuildKey::UIntOp, V_UINT32, V_UINT32, 0, 0
   },
   {
      MUL_ASSIGN_OPERATOR_ID, BuildKey::UIntOp, V_UINT32, V_UINT32, 0, 0
   },
   {
      DIV_ASSIGN_OPERATOR_ID, BuildKey::UIntOp, V_UINT32, V_UINT32, 0, 0
   },
   {
      BAND_OPERATOR_ID, BuildKey::IntOp, V_UINT32, V_UINT32, 0, V_UINT32
   },
   {
      BOR_OPERATOR_ID, BuildKey::IntOp, V_UINT32, V_UINT32, 0, V_UINT32
   },
   {
      BXOR_OPERATOR_ID, BuildKey::IntOp, V_UINT32, V_UINT32, 0, V_UINT32
   },
   {
      BNOT_OPERATOR_ID, BuildKey::IntSOp, V_UINT32, 0, 0, V_UINT32
   },
   {
      INC_OPERATOR_ID, BuildKey::IntSOp, V_UINT32, 0, 0, V_UINT32
   },
   {
      DEC_OPERATOR_ID, BuildKey::IntSOp, V_UINT32, 0, 0, V_UINT32
   },
   {
      SHL_OPERATOR_ID, BuildKey::IntOp, V_UINT32, V_UINT32, 0, V_UINT32
   },
   {
      SHR_OPERATOR_ID, BuildKey::IntOp, V_UINT32, V_UINT32, 0, V_UINT32
   },
   {
      EQUAL_OPERATOR_ID, BuildKey::UIntCondOp, V_UINT32, V_UINT32, 0, V_FLAG
   },
   {
      LESS_OPERATOR_ID, BuildKey::UIntCondOp, V_UINT32, V_UINT32, 0, V_FLAG
   },
   {
      NOTEQUAL_OPERATOR_ID, BuildKey::UIntCondOp, V_UINT32, V_UINT32, 0, V_FLAG
   },
   {
      EQUAL_OPERATOR_ID, BuildKey::UIntCondOp, V_UINT32, V_UINT32, 0, V_FLAG
   },
   {
      LESS_OPERATOR_ID, BuildKey::UIntCondOp, V_UINT32, V_UINT32, 0, V_FLAG
   },
   {
      ADD_OPERATOR_ID, BuildKey::LongOp, V_INT64, V_INT64, 0, V_INT64
   },
   {
      SUB_OPERATOR_ID, BuildKey::LongOp, V_INT64, V_INT64, 0, V_INT64
   },
   {
      MUL_OPERATOR_ID, BuildKey::LongOp, V_INT64, V_INT64, 0, V_INT64
   },
   {
      DIV_OPERATOR_ID, BuildKey::LongOp, V_INT64, V_INT64, 0, V_INT64
   },
   {
      ADD_ASSIGN_OPERATOR_ID, BuildKey::LongOp, V_INT64, V_INT64, 0, 0
   },
   {
      SUB_ASSIGN_OPERATOR_ID, BuildKey::LongOp, V_INT64, V_INT64, 0, 0
   },
   {
      MUL_ASSIGN_OPERATOR_ID, BuildKey::LongOp, V_INT64, V_INT64, 0, 0
   },
   {
      DIV_ASSIGN_OPERATOR_ID, BuildKey::LongOp, V_INT64, V_INT64, 0, 0
   },
   {
      BAND_OPERATOR_ID, BuildKey::LongOp, V_INT64, V_INT64, 0, V_INT64
   },
   {
      BOR_OPERATOR_ID, BuildKey::LongOp, V_INT64, V_INT64, 0, V_INT64
   },
   {
      BXOR_OPERATOR_ID, BuildKey::LongOp, V_INT64, V_INT64, 0, V_INT64
   },
   {
      SHL_OPERATOR_ID, BuildKey::LongOp, V_INT64, V_INT32, 0, V_INT64
   },
   {
      SHR_OPERATOR_ID, BuildKey::LongOp, V_INT64, V_INT32, 0, V_INT64
   },
   {
      ADD_OPERATOR_ID, BuildKey::IntLongOp, V_INT32, V_INT64, 0, V_INT64
   },
   {
      SUB_OPERATOR_ID, BuildKey::IntLongOp, V_INT32, V_INT64, 0, V_INT64
   },
   {
      MUL_OPERATOR_ID, BuildKey::IntLongOp, V_INT32, V_INT64, 0, V_INT64
   },
   {
      DIV_OPERATOR_ID, BuildKey::IntLongOp, V_INT32, V_INT64, 0, V_INT64
   },
   {
      BAND_OPERATOR_ID, BuildKey::IntLongOp, V_INT32, V_INT64, 0, V_INT64
   },
   {
      BOR_OPERATOR_ID, BuildKey::IntLongOp, V_INT32, V_INT64, 0, V_INT64
   },
   {
      BXOR_OPERATOR_ID, BuildKey::IntLongOp, V_INT32, V_INT64, 0, V_INT64
   },
   {
      BNOT_OPERATOR_ID, BuildKey::LongSOp, V_INT64, 0, 0, V_INT64
   },
   {
      NEGATE_OPERATOR_ID, BuildKey::LongSOp, V_INT64, 0, 0, V_INT64
   },
   {
      EQUAL_OPERATOR_ID, BuildKey::LongCondOp, V_INT64, V_INT64, 0, V_FLAG
   },
   {
      LESS_OPERATOR_ID, BuildKey::LongCondOp, V_INT64, V_INT64, 0, V_FLAG
   },
   {
      NOTEQUAL_OPERATOR_ID, BuildKey::LongCondOp, V_INT64, V_INT64, 0, V_FLAG
   },
   {
      EQUAL_OPERATOR_ID, BuildKey::LongCondOp, V_WORD64, V_WORD64, 0, V_FLAG
   },
   {
      NOTEQUAL_OPERATOR_ID, BuildKey::LongCondOp, V_WORD64, V_WORD64, 0, V_FLAG
   },
   {
      EQUAL_OPERATOR_ID, BuildKey::LongIntCondOp, V_INT64, V_INT32, 0, V_FLAG
   },
   {
      NOTEQUAL_OPERATOR_ID, BuildKey::LongIntCondOp, V_INT64, V_INT32, 0, V_FLAG
   },
   {
      LESS_OPERATOR_ID, BuildKey::LongIntCondOp, V_INT64, V_INT32, 0, V_FLAG
   },
   {
      ADD_OPERATOR_ID, BuildKey::ByteOp, V_INT8, V_INT8, 0, V_INT8
   },
   {
      SUB_OPERATOR_ID, BuildKey::ByteOp, V_INT8, V_INT8, 0, V_INT8
   },
   {
      MUL_OPERATOR_ID, BuildKey::ByteOp, V_INT8, V_INT8, 0, V_INT8
   },
   {
      ADD_ASSIGN_OPERATOR_ID, BuildKey::ByteOp, V_INT8, V_INT8, 0, 0
   },
   {
      SUB_ASSIGN_OPERATOR_ID, BuildKey::ByteOp, V_INT8, V_INT8, 0, 0
   },
   {
      MUL_ASSIGN_OPERATOR_ID, BuildKey::ByteOp, V_INT8, V_INT8, 0, 0
   },
   {
      DIV_ASSIGN_OPERATOR_ID, BuildKey::ByteOp, V_INT8, V_INT8, 0, 0
   },
   {
      BAND_OPERATOR_ID, BuildKey::ByteOp, V_INT8, V_INT8, 0, V_INT8
   },
   {
      BOR_OPERATOR_ID, BuildKey::ByteOp, V_INT8, V_INT8, 0, V_INT8
   },
   {
      BXOR_OPERATOR_ID, BuildKey::ByteOp, V_INT8, V_INT8, 0, V_INT8
   },
   {
      BNOT_OPERATOR_ID, BuildKey::ByteSOp, V_INT8, 0, 0, V_INT8
   },
   {
      DIV_OPERATOR_ID, BuildKey::ByteOp, V_INT8, V_INT8, 0, V_INT8
   },
   {
      EQUAL_OPERATOR_ID, BuildKey::ByteCondOp, V_INT8, V_INT8, 0, V_FLAG
   },
   {
      LESS_OPERATOR_ID, BuildKey::ByteCondOp, V_INT8, V_INT8, 0, V_FLAG
   },
   {
      NOTEQUAL_OPERATOR_ID, BuildKey::ByteCondOp, V_INT8, V_INT8, 0, V_FLAG
   },
   {
      SHL_OPERATOR_ID, BuildKey::ByteOp, V_INT8, V_INT32, 0, V_INT8
   },
   {
      SHR_OPERATOR_ID, BuildKey::ByteOp, V_INT8, V_INT32, 0, V_INT8
   },
   {
      ADD_OPERATOR_ID, BuildKey::ByteOp, V_UINT8, V_UINT8, 0, V_UINT8
   },
   {
      SUB_OPERATOR_ID, BuildKey::ByteOp, V_UINT8, V_UINT8, 0, V_UINT8
   },
   {
      MUL_OPERATOR_ID, BuildKey::ByteOp, V_UINT8, V_UINT8, 0, V_UINT8
   },
   {
      ADD_ASSIGN_OPERATOR_ID, BuildKey::ByteOp, V_UINT8, V_UINT8, 0, 0
   },
   {
      SUB_ASSIGN_OPERATOR_ID, BuildKey::ByteOp, V_UINT8, V_UINT8, 0, 0
   },
   {
      MUL_ASSIGN_OPERATOR_ID, BuildKey::ByteOp, V_UINT8, V_UINT8, 0, 0
   },
   {
      DIV_ASSIGN_OPERATOR_ID, BuildKey::ByteOp, V_UINT8, V_UINT8, 0, 0
   },
   {
      BAND_OPERATOR_ID, BuildKey::ByteOp, V_UINT8, V_UINT8, 0, V_UINT8
   },
   {
      BOR_OPERATOR_ID, BuildKey::ByteOp, V_UINT8, V_UINT8, 0, V_UINT8
   },
   {
      BXOR_OPERATOR_ID, BuildKey::ByteOp, V_UINT8, V_UINT8, 0, V_UINT8
   },
   {
      BNOT_OPERATOR_ID, BuildKey::ByteSOp, V_UINT8, 0, 0, V_UINT8
   },
   {
      DIV_OPERATOR_ID, BuildKey::ByteOp, V_UINT8, V_UINT8, 0, V_UINT8
   },
   {
      EQUAL_OPERATOR_ID, BuildKey::ByteCondOp, V_UINT8, V_UINT8, 0, V_FLAG
   },
   {
      LESS_OPERATOR_ID, BuildKey::UByteCondOp, V_UINT8, V_UINT8, 0, V_FLAG
   },
   {
      NOTEQUAL_OPERATOR_ID, BuildKey::ByteCondOp, V_UINT8, V_UINT8, 0, V_FLAG
   },
   {
      SHL_OPERATOR_ID, BuildKey::ByteOp, V_UINT8, V_INT32, 0, V_UINT8
   },
   {
      SHR_OPERATOR_ID, BuildKey::ByteOp, V_UINT8, V_INT32, 0, V_UINT8
   },
   {
      ADD_ASSIGN_OPERATOR_ID, BuildKey::ShortOp, V_INT16, V_INT16, 0, 0
   },
   {
      SUB_ASSIGN_OPERATOR_ID, BuildKey::ShortOp, V_INT16, V_INT16, 0, 0
   },
   {
      MUL_ASSIGN_OPERATOR_ID, BuildKey::ShortOp, V_INT16, V_INT16, 0, 0
   },
   {
      DIV_ASSIGN_OPERATOR_ID, BuildKey::ShortOp, V_INT16, V_INT16, 0, 0
   },
   {
      ADD_OPERATOR_ID, BuildKey::ShortOp, V_INT16, V_INT16, 0, V_INT16
   },
   {
      SUB_OPERATOR_ID, BuildKey::ShortOp, V_INT16, V_INT16, 0, V_INT16
   },
   {
      MUL_OPERATOR_ID, BuildKey::ShortOp, V_INT16, V_INT16, 0, V_INT16
   },
   {
      DIV_OPERATOR_ID, BuildKey::ShortOp, V_INT16, V_INT16, 0, V_INT16
   },
   {
      BAND_OPERATOR_ID, BuildKey::ShortOp, V_INT16, V_INT16, 0, V_INT16
   },
   {
      BOR_OPERATOR_ID, BuildKey::ShortOp, V_INT16, V_INT16, 0, V_INT16
   },
   {
      BXOR_OPERATOR_ID, BuildKey::ShortOp, V_INT16, V_INT16, 0, V_INT16
   },
   {
      BNOT_OPERATOR_ID, BuildKey::ShortSOp, V_INT16, 0, 0, V_INT16
   },
   {
      SHL_OPERATOR_ID, BuildKey::ShortOp, V_INT16, V_INT32, 0, V_INT16
   },
   {
      SHR_OPERATOR_ID, BuildKey::ShortOp, V_INT16, V_INT32, 0, V_INT16
   },
   {
      EQUAL_OPERATOR_ID, BuildKey::ShortCondOp, V_INT16, V_INT16, 0, V_FLAG
   },
   {
      LESS_OPERATOR_ID, BuildKey::ShortCondOp, V_INT16, V_INT16, 0, V_FLAG
   },
   {
      NOTEQUAL_OPERATOR_ID, BuildKey::ShortCondOp, V_INT16, V_INT16, 0, V_FLAG
   },
   {
      ADD_OPERATOR_ID, BuildKey::ShortOp, V_UINT16, V_UINT16, 0, V_UINT16
   },
   {
      SUB_OPERATOR_ID, BuildKey::ShortOp, V_UINT16, V_UINT16, 0, V_UINT16
   },
   {
      MUL_OPERATOR_ID, BuildKey::ShortOp, V_UINT16, V_UINT16, 0, V_UINT16
   },
   {
      DIV_OPERATOR_ID, BuildKey::ShortOp, V_UINT16, V_UINT16, 0, V_UINT16
   },
   {
      ADD_ASSIGN_OPERATOR_ID, BuildKey::ShortOp, V_UINT16, V_UINT16, 0, 0
   },
   {
      SUB_ASSIGN_OPERATOR_ID, BuildKey::ShortOp, V_UINT16, V_UINT16, 0, 0
   },
   {
      MUL_ASSIGN_OPERATOR_ID, BuildKey::ShortOp, V_UINT16, V_UINT16, 0, 0
   },
   {
      DIV_ASSIGN_OPERATOR_ID, BuildKey::ShortOp, V_UINT16, V_UINT16, 0, 0
   },
   {
      BAND_OPERATOR_ID, BuildKey::ShortOp, V_UINT16, V_UINT16, 0, V_UINT16
   },
   {
      BOR_OPERATOR_ID, BuildKey::ShortOp, V_UINT16, V_UINT16, 0, V_UINT16
   },
   {
      BXOR_OPERATOR_ID, BuildKey::ShortOp, V_UINT16, V_UINT16, 0, V_UINT16
   },
   {
      BNOT_OPERATOR_ID, BuildKey::ShortSOp, V_UINT16, 0, 0, V_UINT16
   },
   {
      SHL_OPERATOR_ID, BuildKey::ShortOp, V_UINT16, V_INT32, 0, V_UINT16
   },
   {
      SHR_OPERATOR_ID, BuildKey::ShortOp, V_UINT16, V_INT32, 0, V_UINT16
   },
   {
      EQUAL_OPERATOR_ID, BuildKey::ShortCondOp, V_UINT16, V_UINT16, 0, V_FLAG
   },
   {
      LESS_OPERATOR_ID, BuildKey::UShortCondOp, V_UINT16, V_UINT16, 0, V_FLAG
   },
   {
      NOTEQUAL_OPERATOR_ID, BuildKey::ShortCondOp, V_UINT16, V_UINT16, 0, V_FLAG
   },
   {
      ADD_OPERATOR_ID, BuildKey::RealOp, V_FLOAT64, V_FLOAT64, 0, V_FLOAT64
   },
   {
      SUB_OPERATOR_ID, BuildKey::RealOp, V_FLOAT64, V_FLOAT64, 0, V_FLOAT64
   },
   {
      MUL_OPERATOR_ID, BuildKey::RealOp, V_FLOAT64, V_FLOAT64, 0, V_FLOAT64
   },
   {
      DIV_OPERATOR_ID, BuildKey::RealOp, V_FLOAT64, V_FLOAT64, 0, V_FLOAT64
   },
   {
      ADD_ASSIGN_OPERATOR_ID, BuildKey::RealOp, V_FLOAT64, V_FLOAT64, 0, 0
   },
   {
      SUB_ASSIGN_OPERATOR_ID, BuildKey::RealOp, V_FLOAT64, V_FLOAT64, 0, 0
   },
   {
      MUL_ASSIGN_OPERATOR_ID, BuildKey::RealOp, V_FLOAT64, V_FLOAT64, 0, 0
   },
   {
      DIV_ASSIGN_OPERATOR_ID, BuildKey::RealOp, V_FLOAT64, V_FLOAT64, 0, 0
   },
   {
      EQUAL_OPERATOR_ID, BuildKey::RealCondOp, V_FLOAT64, V_FLOAT64, 0, V_FLAG
   },
   {
      LESS_OPERATOR_ID, BuildKey::RealCondOp, V_FLOAT64, V_FLOAT64, 0, V_FLAG
   },
   {
      NOTEQUAL_OPERATOR_ID, BuildKey::RealCondOp, V_FLOAT64, V_FLOAT64, 0, V_FLAG
   },
   {
      NOT_OPERATOR_ID, BuildKey::BoolSOp, V_FLAG, 0, 0, V_FLAG
   },
   {
      // NOTE : the output should be in the stack, aligned to the 4 / 8 bytes
      INDEX_OPERATOR_ID, BuildKey::ByteArrayOp, V_INT8ARRAY, V_INT32, 0, V_ELEMENT
   },
   {
      SET_INDEXER_OPERATOR_ID, BuildKey::ByteArrayOp, V_INT8ARRAY, V_ELEMENT, V_INT32, 0
   },
   {
      LEN_OPERATOR_ID, BuildKey::ByteArraySOp, V_INT8ARRAY, 0, 0, V_INT32
   },
   {
      LEN_OPERATOR_ID, BuildKey::ShortArraySOp, V_INT16ARRAY, 0, 0, V_INT32
   },
   {
      // NOTE : the output should be in the stack, aligned to the 4 / 8 bytes
      INDEX_OPERATOR_ID, BuildKey::ShortArrayOp, V_INT16ARRAY, V_INT32, 0, V_ELEMENT
   },
   {
      SET_INDEXER_OPERATOR_ID, BuildKey::ShortArrayOp, V_INT16ARRAY, V_ELEMENT, V_INT32, 0
   },
   {
      LEN_OPERATOR_ID, BuildKey::IntArraySOp, V_INT32ARRAY, 0, 0, V_INT32
   },
   {
      // NOTE : the output should be in the stack, aligned to the 4 / 8 bytes
      INDEX_OPERATOR_ID, BuildKey::IntArrayOp, V_INT32ARRAY, V_INT32, 0, V_ELEMENT
   },
   {
      SET_INDEXER_OPERATOR_ID, BuildKey::IntArrayOp, V_INT32ARRAY, V_ELEMENT, V_INT32, 0
   },
   {
      LEN_OPERATOR_ID, BuildKey::BinaryArraySOp, V_FLOAT64ARRAY, 0, 0, V_INT32
   },
   {
      // NOTE : the output should be in the stack, aligned to the 4 / 8 bytes
      INDEX_OPERATOR_ID, BuildKey::BinaryArrayOp, V_FLOAT64ARRAY, V_INT32, 0, V_ELEMENT
   },
   {
      SET_INDEXER_OPERATOR_ID, BuildKey::BinaryArrayOp, V_FLOAT64ARRAY, V_ELEMENT, V_INT32, 0
   },
   {
      INDEX_OPERATOR_ID, BuildKey::BinaryArrayOp, V_BINARYARRAY, V_INT32, 0, V_ELEMENT
   },
   {
      SET_INDEXER_OPERATOR_ID, BuildKey::BinaryArrayOp, V_BINARYARRAY, V_ELEMENT, V_INT32, 0
   },
   {
      LEN_OPERATOR_ID, BuildKey::BinaryArraySOp, V_BINARYARRAY, 0, 0, V_INT32
   },
   {
      IF_OPERATOR_ID, BuildKey::BranchOp, V_FLAG, V_CLOSURE, 0, V_CLOSURE
   },
   {
      ELSE_OPERATOR_ID, BuildKey::BranchOp, V_FLAG, V_CLOSURE, 0, V_CLOSURE
   },
   {
      IF_ELSE_OPERATOR_ID, BuildKey::BranchOp, V_FLAG, V_CLOSURE, V_CLOSURE, V_CLOSURE
   },
   {
      IF_ELSE_OPERATOR_ID, BuildKey::BranchOp, V_FLAG, V_OBJECT, V_OBJECT, V_OBJECT
   },
   {
      LEN_OPERATOR_ID, BuildKey::ObjArraySOp, V_OBJARRAY, 0, 0, V_INT32
   },
   {
      INDEX_OPERATOR_ID, BuildKey::ObjArrayOp, V_OBJARRAY, V_INT32, 0, V_ELEMENT
   },
   {
      SET_INDEXER_OPERATOR_ID, BuildKey::ObjArrayOp, V_OBJARRAY, V_ELEMENT, V_INT32, 0
   },
   {
      INDEX_OPERATOR_ID, BuildKey::ObjArrayOp, V_ARGARRAY, V_INT32, 0, V_ELEMENT
   },
   {
      ADD_OPERATOR_ID, BuildKey::IntRealOp, V_INT32, V_FLOAT64, 0, V_FLOAT64
   },
   {
      SUB_OPERATOR_ID, BuildKey::IntRealOp, V_INT32, V_FLOAT64, 0, V_FLOAT64
   },
   {
      MUL_OPERATOR_ID, BuildKey::IntRealOp, V_INT32, V_FLOAT64, 0, V_FLOAT64
   },
   {
      DIV_OPERATOR_ID, BuildKey::IntRealOp, V_INT32, V_FLOAT64, 0, V_FLOAT64
   },
   {
      ADD_OPERATOR_ID, BuildKey::RealIntOp, V_FLOAT64, V_INT32, 0, V_FLOAT64
   },
   {
      SUB_OPERATOR_ID, BuildKey::RealIntOp, V_FLOAT64, V_INT32, 0, V_FLOAT64
   },
   {
      MUL_OPERATOR_ID, BuildKey::RealIntOp, V_FLOAT64, V_INT32, 0, V_FLOAT64
   },
   {
      DIV_OPERATOR_ID, BuildKey::RealIntOp, V_FLOAT64, V_INT32, 0, V_FLOAT64
   },
   {
      ADD_ASSIGN_OPERATOR_ID, BuildKey::RealIntOp, V_FLOAT64, V_INT32, 0, 0
   },
   {
      SUB_ASSIGN_OPERATOR_ID, BuildKey::RealIntOp, V_FLOAT64, V_INT32, 0, 0
   },
   {
      MUL_ASSIGN_OPERATOR_ID, BuildKey::RealIntOp, V_FLOAT64, V_INT32, 0, 0
   },
   {
      DIV_ASSIGN_OPERATOR_ID, BuildKey::RealIntOp, V_FLOAT64, V_INT32, 0, 0
   },
};

bool CompilerLogic :: isPrimitiveCompatible(ModuleScopeBase& scope, TypeInfo target, TypeInfo source)
{
   if (target == source)
      return true;

   switch (target.typeRef) {
      case V_OBJECT:
         return !isPrimitiveRef(source.typeRef);
      case V_INT32:
         return source.typeRef == V_WORD32 || source.typeRef == V_MESSAGE || source.typeRef == V_PTR32 || source.typeRef == V_MESSAGENAME;
      case V_INT64:
         return source.typeRef == V_PTR64 || source.typeRef == V_WORD64;
      case V_FLAG:
         return isCompatible(scope, { scope.branchingInfo.typeRef }, source, true);
      case V_WORD32:
         return source.typeRef == V_INT32;
      case V_WORD64:
         return source.typeRef == V_INT64;
      default:
         return false;
   }
}

// --- CompilerLogic ---

void CompilerLogic :: loadOperations()
{
   for (size_t i = 0; i < OperationLength; i++) {
      _operations.add(Operations[i].operatorId, Operations[i]);
   }
}

bool CompilerLogic :: isValidOp(int operatorId, const int* validOperators, size_t len)
{
   for (size_t i = 0; i < len; ++i) {
      if (validOperators[i] == operatorId)
         return true;
   }

   return false;
}

BuildKey CompilerLogic :: resolveOp(ModuleScopeBase& scope, int operatorId, ref_t* arguments, size_t length,
   ref_t& outputRef)
{
   auto it = _operations.getIt(operatorId);
   while (!it.eof()) {
      Op op = *it;

      bool compatible = isCompatible(scope, { op.loperand }, { arguments[0] }, false);
      compatible = compatible && (length <= 1 || isCompatible(scope, { op.roperand }, { arguments[1] }, false));
      compatible = compatible && (length <= 2 || isCompatible(scope, { op.ioperand }, { arguments[2] }, false));
      if (compatible) {
         outputRef = op.output;

         return op.operation;
      }

      it = _operations.nextIt(operatorId, it);
   }

   return BuildKey::None;
}

BuildKey CompilerLogic :: resolveNewOp(ModuleScopeBase& scope, ref_t loperand, ref_t* signatures, pos_t signatureLen)
{
   if (signatureLen == 1 &&
      isCompatible(scope, { V_INT32 }, { signatures[0] }, false))
   {
      ClassInfo info;
      if (defineClassInfo(scope, info, loperand, true)) {
         return test(info.header.flags, elDynamicRole) ? BuildKey::NewArrayOp : BuildKey::None;
      }
   }

   return BuildKey::None;
}

bool CompilerLogic :: validateTemplateAttribute(ref_t attribute, Visibility& visibility, TemplateType& type)
{
   switch (attribute) {
      case V_PUBLIC:
         visibility = Visibility::Public;
         break;
      case V_PRIVATE:
         visibility = Visibility::Private;
         break;
      case V_INLINE:
         type = TemplateType::Inline;
         break;
      case V_FIELD:
         type = TemplateType::InlineProperty;
         break;
      case V_ENUMERATION:
         type = TemplateType::Enumeration;
         break;
      case V_TEXTBLOCK:
         type = TemplateType::ClassBlock;
         break;
      case V_TEMPLATE:
      case V_WEAK:
         break;
      default:
      {
         ref_t dummy = 0;
         return validateClassAttribute(attribute, dummy, visibility);
      }
   }

   return true;
}

bool CompilerLogic :: validateSymbolAttribute(ref_t attribute, Visibility& visibility, bool& constant, SymbolKind& symbolKind)
{
   switch (attribute) {
      case V_PUBLIC:
         visibility = Visibility::Public;
         break;
      case V_INTERNAL:
         visibility = Visibility::Internal;
         break;
      case V_PRIVATE:
         visibility = Visibility::Private;
         break;
      case V_SYMBOLEXPR:
         break;
      case V_CONST:
         constant = true;
         break;
      case V_STATIC:
         symbolKind = SymbolKind::Static;
         break;
      case V_THREADVAR:
         symbolKind = SymbolKind::ThreadVar;
         break;
      case 0:
         // ignore idle
         break;
      default:
         return false;
   }

   return true;
}

bool CompilerLogic :: validateClassAttribute(ref_t attribute, ref_t& flags, Visibility& visibility)
{
   switch (attribute) {
      case V_PUBLIC:
         visibility = Visibility::Public;
         break;
      case V_PRIVATE:
         visibility = Visibility::Private;
         break;
      case V_INTERNAL:
         visibility = Visibility::Internal;
         break;
      case V_CLASS:
         break;
      case V_STRUCT:
         flags |= elStructureRole;
         break;
      case V_CONST:
         flags |= elReadOnlyRole;
         return true;
      case V_SINGLETON:
         flags |= elRole | elSealed | elStateless;
         break;
      case V_INTERFACE:
         if (!(flags & elDebugMask)) {
            flags |= (elClosed | elAbstract | elNoCustomDispatcher);
            flags |= elInterface;
         }
         else return false;
         break;
      case V_ABSTRACT:
         flags |= elAbstract;
         break;
      case V_SEALED:
         flags |= elSealed;
         break;
      case V_CLOSED:
         flags |= elClosed;
         break;
      case V_EXTENSION:
         flags |= elExtension;
         break;
      case V_NONESTRUCT:
         flags |= elNonStructureRole;
         break;
      case V_TEMPLATEBASED:
         flags |= elTemplatebased;
         break;
      case V_MIXIN:
         flags |= elGroup;
         break;
      case V_PACKED_STRUCT:
         flags |= elPacked | elStructureRole;
         break;
      case 0:
         // ignore idle
         break;
      default:
         return false;
   }

   return true;
}

bool CompilerLogic :: validateFieldAttribute(ref_t attribute, FieldAttributes& attrs)
{
   switch (attribute) {
      case V_FIELD:
         break;
      case V_INTBINARY:
      case V_UINTBINARY:
      case V_WORDBINARY:
      case V_MSSGBINARY:
      case V_SUBJBINARY:
      case V_FLOATBINARY:
      case V_POINTER:
      case V_EXTMESSAGE:
         attrs.typeInfo.typeRef = attribute;
         break;
      case V_STRINGOBJ:
         attrs.inlineArray = true;
         break;
      case V_EMBEDDABLE:
         attrs.isEmbeddable = true;
         break;
      case V_CONST:
         attrs.isConstant = true;
         break;
      case V_STATIC:
         attrs.isStatic = true;
         break;
      case V_THREADVAR:
         attrs.isThreadStatic = true;
         break;
      case V_READONLY:
         attrs.isReadonly = true;
         break;
      case V_OVERRIDE:
         attrs.overrideMode = true;
         break;
      case V_PRIVATE:
         attrs.privateOne = true;
         break;
      default:
         return false;
   }

   return true;
}

bool CompilerLogic :: validateMethodAttribute(ref_t attribute, ref_t& hint, bool& explicitMode)
{
   switch (attribute) {
      case 0:
      case V_PUBLIC:
         break;
      case V_PRIVATE:
         hint = (ref_t)MethodHint::Private | (ref_t)MethodHint::Sealed;
         break;
      case V_PROTECTED:
         hint = (ref_t)MethodHint::Protected;
         break;
      case V_INTERNAL:
         hint = (ref_t)MethodHint::Internal;
         break;
      case V_METHOD:
         explicitMode = true;
         break;
      case V_STATIC:
         hint = (ref_t)MethodHint::Static;
         break;
      case V_DISPATCHER:
         explicitMode = true;
         hint = (ref_t)MethodHint::Dispatcher;
         break;
      case V_CONSTRUCTOR:
         explicitMode = true;
         hint = (ref_t)MethodHint::Constructor;
         break;
      case V_ABSTRACT:
         hint = (ref_t)MethodHint::Abstract;
         break;
      case V_GETACCESSOR:
         hint = (ref_t)MethodHint::GetAccessor;
         break;
      case V_SETACCESSOR:
         hint = (ref_t)MethodHint::SetAccessor;
         break;
      case V_CONST:
         hint = (ref_t)MethodHint::Constant | (ref_t)MethodHint::Sealed;
         return true;
      case V_FUNCTION:
         hint = (ref_t)MethodHint::Function;
         return true;
      case V_PREDEFINED:
         hint = (ref_t)MethodHint::Predefined;
         return true;
      case V_CONVERSION:
         hint = (ref_t)MethodHint::Conversion;
         return true;
      case V_OVERLOADRET:
         hint = (ref_t)MethodHint::RetOverload;
         return true;
      case V_SEALED:
         hint = (ref_t)MethodHint::Sealed;
         return true;
      case V_GENERIC:
         hint = (ref_t)MethodHint::Sealed | (ref_t)MethodHint::Generic;
         return true;
      case V_INTERFACE_DISPATCHER:
         hint = (ref_t)MethodHint::InterfaceDispatcher;
         return true;
      case V_MIXIN:
         hint = (ref_t)MethodHint::Mixin;
         return true;
      case V_SCRIPTSELFMODE:
         hint = (ref_t)MethodHint::TargetSelf;
         return true;
      case V_YIELDABLE:
         hint = (ref_t)MethodHint::Yieldable;
         return true;
      case V_ASYNC:
         hint = (ref_t)MethodHint::Async;
         return true;
      case V_INDEXED_ATTR:
         hint = (ref_t)MethodHint::Indexed;
         return true;
      default:
         return false;
   }

   return true;
}

bool CompilerLogic :: validateImplicitMethodAttribute(ref_t attribute, ref_t& hint)
{
   bool dummy = false;
   switch (attribute) {
      case V_METHOD:
      case V_DISPATCHER:
      case V_CONSTRUCTOR:
      case V_FUNCTION:
      case V_CONVERSION:
      case V_GENERIC:
         return validateMethodAttribute(attribute, hint, dummy);
      default:
         return false;
   }
}

bool CompilerLogic :: validateDictionaryAttribute(ref_t attribute, TypeInfo& dictionaryTypeInfo, bool& superMode)
{
   switch (attribute) {
      case V_STRINGOBJ:
         dictionaryTypeInfo.typeRef = V_STRINGOBJ;
         dictionaryTypeInfo.elementRef = V_SYMBOL;
         return true;
      case V_SYMBOL:
         dictionaryTypeInfo.typeRef = V_DICTIONARY;
         dictionaryTypeInfo.elementRef = V_SYMBOL;
         return true;
      //case V_DECLOBJ:
      //   dictionaryType = V_DECLATTRIBUTES;
      //   return true;
      case V_SUPERIOR:
         superMode = true;
         return true;
      default:
         return false;
   }
}

bool CompilerLogic :: validateExpressionAttribute(ref_t attrValue, ExpressionAttributes& attrs)
{
   switch(attrValue) {
      case V_FORWARD:
         attrs |= ExpressionAttribute::Forward;
         return true;
      case V_DISTRIBUTED_FORWARD:
         attrs |= ExpressionAttribute::DistributedForward;
         return true;
      case V_INTERN:
         attrs |= ExpressionAttribute::Intern;
         return true;
      case V_AUTO:
      case V_VARIABLE:
         attrs |= ExpressionAttribute::NewVariable;
         return true;
      case V_EXTERN:
         attrs |= ExpressionAttribute::Extern;
         return true;
      case V_NEWOP:
         if (ExpressionAttributes::test(attrs.attrs, ExpressionAttribute::Parameter)
            || ExpressionAttributes::test(attrs.attrs, ExpressionAttribute::NestedDecl)
            || ExpressionAttributes::test(attrs.attrs, ExpressionAttribute::Meta))
         {
            attrs |= ExpressionAttribute::NewOp;
            return true;
         }
         return false;
      case V_CONVERSION:
         attrs |= ExpressionAttribute::CastOp;
         return true;
      case V_WRAPPER:
         attrs |= ExpressionAttribute::RefOp;
         return true;
      case V_OUTWRAPPER:
         attrs |= ExpressionAttribute::OutRefOp;
         return true;
      case V_MSSGNAME:
         attrs |= ExpressionAttribute::MssgNameLiteral;
         return true;
      case V_PROBEMODE:
         attrs |= ExpressionAttribute::ProbeMode;
         return true;
      case V_MEMBER:
         attrs |= ExpressionAttribute::Member;
         return true;
      case V_WEAK:
         attrs |= ExpressionAttribute::Weak;
         return true;
      case V_SUPERIOR:
         attrs |= ExpressionAttribute::Superior;
         return true;
      case V_IGNOREDUPLICATE:
         attrs |= ExpressionAttribute::IgnoreDuplicate;
         return true;
      case V_VARIADIC:
         attrs |= ExpressionAttribute::Variadic;
         return true;
      case V_TYPEOF:
         attrs |= ExpressionAttribute::RetrievingType;
         return true;
      case V_CLASS:
         attrs |= ExpressionAttribute::Class;
         return true;
      default:
         return false;
   }
}

bool CompilerLogic :: validateArgumentAttribute(ref_t attrValue, TypeAttributes& attributes)
{
   switch (attrValue) {
      case V_WRAPPER:
         attributes.byRefOne = true;
         return true;
      case V_OUTWRAPPER:
         attributes.outRefOne = true;
         return true;
      case V_VARIADIC:
         attributes.variadicOne = true;
         return true;
      case V_VARIABLE:
         return true;
      default:
         return false;
   }
}

bool CompilerLogic :: validateTypeScopeAttribute(ref_t attrValue, TypeAttributes& attributes)
{
   switch (attrValue) {
      case V_VARIADIC:
         attributes.variadicOne = true;
         return true;
      case V_VARIABLE:
         attributes.variableOne = true;
         return true;
      case V_MSSGNAME:
         attributes.mssgNameLiteral = true;
         return true;
      case V_NEWOP:
         attributes.newOp = true;
         return true;
      case V_CLASS:
         attributes.classOne = true;
         return true;
      case V_CONVERSION:
         attributes.typecastOne = true;
         return true;
      default:
         return false;
   }
}

bool CompilerLogic :: validateIncludeAttribute(ref_t attrValue, bool& textBlock)
{
   switch (attrValue) {
      case V_TEXTBLOCK:
         textBlock = true;
         return true;
      default:
         return false;
   }
}

bool CompilerLogic :: validateResendAttribute(ref_t attrValue, bool& superMode)
{
   switch (attrValue) {
      case V_SUPERIOR:
         superMode = true;
         return true;
      default:
         return false;
   }
}

bool CompilerLogic :: validateMessage(ModuleScopeBase& scope, ref_t hints, mssg_t message)
{
   bool dispatchOne = message == scope.buildins.dispatch_message;
   if (testany((int)hints, (int)(MethodHint::Constructor | MethodHint::Static))) {
      if (dispatchOne)
         return false;
   }

   // const attribute can be applied only to a get-property
   if (testMethodHint(hints, MethodHint::Constant)
      && ((message & PREFIX_MESSAGE_MASK) != PROPERTY_MESSAGE && getArgCount(message) > 1))
   {
      return false;
   }

   return true;
}

inline bool existsNormalMethod(ModuleScopeBase& scope, ClassInfo& info, mssg_t variadicMssg)
{
   ref_t dummy = 0;
   ref_t actionRef = getAction(variadicMssg);
   ustr_t actionName = scope.module->resolveAction(actionRef, dummy);
   if (actionName.compare(CONSTRUCTOR_MESSAGE) || actionName.compare(CONSTRUCTOR_MESSAGE2))
      return false;

   for (auto it = info.methods.start(); !it.eof(); ++it) {
      auto mssg = it.key();

      if ((mssg & PREFIX_MESSAGE_MASK) != VARIADIC_MESSAGE) {
         ustr_t currentActionName = scope.module->resolveAction(getAction(mssg), dummy);

         if (currentActionName.compare(actionName))
            return true;
      }
   }

   return false;
}

void CompilerLogic :: validateClassDeclaration(ModuleScopeBase& scope, ErrorProcessorBase* errorProcessor, ClassInfo& info,
   bool& emptyStructure, bool& dispatcherNotAllowed, bool& withAbstractMethods, mssg_t& mixedUpVariadicMessage)
{
   bool abstractOne = isAbstract(info);
   bool withVariadic = withVariadicsMethods(info);

   // check abstract methods
   if (!abstractOne || withVariadic) {
      for (auto it = info.methods.start(); !it.eof(); ++it) {
         auto mssg = it.key();
         auto methodInfo = *it;

         if (!abstractOne && test(methodInfo.hints, (ref_t)MethodHint::Abstract)) {
            IdentifierString messageName;
            ByteCodeUtil::resolveMessageName(messageName, scope.module, mssg);

            errorProcessor->info(infoAbstractMetod, *messageName);

            withAbstractMethods = true;
         }
         if (withVariadic && ((mssg & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE)) {
            if (existsNormalMethod(scope, info, mssg)) {
               // NOTE : check only the first mismatched variadic message
               mixedUpVariadicMessage = mssg;
               withVariadic = false;
            }
         }
      }
   }

   // a structure class should contain fields
   if (test(info.header.flags, elStructureRole) && info.size == 0)
      emptyStructure = true;

   // interface class cannot have a custom dispatcher method
   if (test(info.header.flags, elNoCustomDispatcher)) {
      auto dispatchInfo = info.methods.get(scope.buildins.dispatch_message);

      dispatcherNotAllowed = !dispatchInfo.inherited;
   }
}

bool CompilerLogic :: validateAutoType(ModuleScopeBase& scope, TypeInfo& typeInfo)
{
   ClassInfo info;
   if (!defineClassInfo(scope, info, typeInfo.typeRef))
      return false;

   while (isRole(info)) {
      typeInfo = { info.header.parentRef };

      if (!defineClassInfo(scope, info, typeInfo.typeRef))
         return false;
   }

   return true;
}

bool CompilerLogic:: isTryDispatchAllowed(ModuleScopeBase& scope, mssg_t message)
{
   return message == overwriteArgCount(scope.buildins.invoke_message, 1);
}

mssg_t CompilerLogic :: defineTryDispatcher(ModuleScopeBase& scope, mssg_t message)
{
   return encodeMessage(scope.module->mapAction(TRY_INVOKE_MESSAGE, 0, false), 2, FUNCTION_MESSAGE);
}

ref_t CompilerLogic :: defineByRefSignature(ModuleScopeBase& scope, ref_t signRef, ref_t resultRef)
{
   ref_t targetSignatures[ARG_COUNT];

   size_t len = signRef != 0 ? scope.module->resolveSignature(signRef, targetSignatures) : 0;
   targetSignatures[len++] = resultRef;

   return scope.module->mapSignature(targetSignatures, len, false);
}

bool CompilerLogic :: isRole(ClassInfo& info)
{
   return test(info.header.flags, elRole);
}

bool CompilerLogic :: isAbstract(ClassInfo& info)
{
   return test(info.header.flags, elAbstract);
}

bool CompilerLogic :: withVariadicsMethods(ClassInfo& info)
{
   return test(info.header.flags, elWithVariadics);
}

bool CompilerLogic :: isReadOnly(ClassInfo& info)
{
   return test(info.header.flags, elReadOnlyRole);
}

bool CompilerLogic :: isEmbeddableArray(ModuleScopeBase& scope, ref_t reference)
{
   if (scope.cachedEmbeddableArrays.exist(reference))
      return scope.cachedEmbeddableArrays.get(reference);

   ClassInfo info;
   if (defineClassInfo(scope, info, reference, true)) {
      auto retVal = isEmbeddableArray(info);

	   scope.cachedEmbeddableArrays.add(reference, retVal);

	   return retVal;
   }

   return false;
}

bool CompilerLogic :: isDynamic(ClassInfo& info)
{
   return test(info.header.flags, elDynamicRole | elWrapper);
}

bool CompilerLogic :: isEmbeddableArray(ClassInfo& info)
{
   return test(info.header.flags, elDynamicRole | elStructureRole | elWrapper);
}

bool CompilerLogic :: isEmbeddableStruct(ClassInfo& info)
{
   return test(info.header.flags, elStructureRole)
      && !test(info.header.flags, elDynamicRole);
}

bool CompilerLogic :: isEmbeddable(ModuleScopeBase& scope, TypeInfo typeInfo)
{
   if (typeInfo.nillable)
      return false;

   if (scope.cachedEmbeddables.exist(typeInfo.typeRef))
      return scope.cachedEmbeddables.get(typeInfo.typeRef);

   ClassInfo info;
   if (defineClassInfo(scope, info, typeInfo.typeRef, true)) {
      auto retVal = isEmbeddable(info);

      scope.cachedEmbeddables.add(typeInfo.typeRef, retVal);

      return retVal;
   }

   return false;
}

bool CompilerLogic :: isEmbeddable(ClassInfo& info)
{
   if (test(info.header.flags, elDynamicRole)) {
      return isEmbeddableArray(info);
   }
   else return isEmbeddableStruct(info);
}

bool CompilerLogic :: isEmbeddableAndReadOnly(ClassInfo& info)
{
   return isReadOnly(info) && isEmbeddable(info);
}

bool CompilerLogic::isEmbeddableAndReadOnly(ModuleScopeBase& scope, TypeInfo typeInfo)
{
   if (typeInfo.nillable)
      return false;

   if (scope.cachedEmbeddableReadonlys.exist(typeInfo.typeRef))
      return scope.cachedEmbeddableReadonlys.get(typeInfo.typeRef);

   ClassInfo info;
   if (defineClassInfo(scope, info, typeInfo.typeRef, true)) {
      auto retVal = isEmbeddableAndReadOnly(info);

      scope.cachedEmbeddableReadonlys.add(typeInfo.typeRef, retVal);

      return retVal;
   }

   return false;
}

bool CompilerLogic :: isEmbeddableStruct(ModuleScopeBase& scope, TypeInfo typeInfo)
{
   if (typeInfo.nillable)
      return false;

   if (scope.cachedEmbeddableStructs.exist(typeInfo.typeRef))
	  return scope.cachedEmbeddableStructs.get(typeInfo.typeRef);

   ClassInfo info;
   if (defineClassInfo(scope, info, typeInfo.typeRef, true)) {
      auto retVal = isEmbeddableStruct(info);

      scope.cachedEmbeddableStructs.add(typeInfo.typeRef, retVal);

      return retVal;
   }

   return false;
}

bool CompilerLogic :: isStacksafeArg(ClassInfo& info)
{
   if (test(info.header.flags, elDynamicRole)) {
      return isEmbeddableArray(info);
   }
   else return isEmbeddable(info);
}

bool CompilerLogic :: isStacksafeArg(ModuleScopeBase& scope, ref_t reference)
{
   if (scope.cachedStacksafeArgs.exist(reference))
	  return scope.cachedStacksafeArgs.get(reference);

   ClassInfo info;
   if (defineClassInfo(scope, info, reference, true)) {
      auto retVal = isStacksafeArg(info);

	  scope.cachedStacksafeArgs.add(reference, retVal);

      return retVal;
   }

   return false;
}

bool CompilerLogic :: isWrapper(ModuleScopeBase& scope, ref_t reference)
{
   if (scope.cachedWrappers.exist(reference))
	   return scope.cachedWrappers.get(reference);

   ClassInfo info;
   if (defineClassInfo(scope, info, reference, true)) {
       auto retVal = isWrapper(info);

       scope.cachedWrappers.add(reference, retVal);

       return retVal;
   }

   return false;
}

bool CompilerLogic :: isWrapper(ClassInfo& info)
{
   return test(info.header.flags, elWrapper)
      && !test(info.header.flags, elDynamicRole);
}

bool CompilerLogic :: isMultiMethod(ClassInfo& info, MethodInfo& methodInfo)
{
   return test(methodInfo.hints, (ref_t)MethodHint::Multimethod);
}

void CompilerLogic :: tweakClassFlags(ModuleScopeBase& scope, ref_t classRef, ClassInfo& info, bool classClassMode)
{
   if (classClassMode) {
      // class class is always stateless and final
      info.header.flags |= elStateless;
      info.header.flags |= elSealed;
   }

   if (test(info.header.flags, elNestedClass)) {
      // stateless inline class
      if (info.fields.count() == 0 && !test(info.header.flags, elStructureRole)) {
         info.header.flags |= elStateless;

         // stateless inline class is its own class class
         info.header.classRef = classRef;
      }
      else info.header.flags &= ~elStateless;
   }

   if (test(info.header.flags, elExtension))
      info.header.flags |= elSealed;

   if (test(info.header.flags, elDynamicRole)) {
      if (test(info.header.flags, elStructureRole)) {
         if (classRef == scope.buildins.literalReference) {
            // recognize string constant
            if (info.size == -1) {
               info.header.flags |= elDebugLiteral;
            }
         }
         else if (classRef == scope.buildins.wideReference) {
            // recognize wide string constant
            if (info.size == -2) {
               info.header.flags |= elDebugWideLiteral;
            }
         }
      }
      else {
         info.header.flags |= elDebugArray;
      }
   }

   if (isEmbeddableArray(info)) {
      auto inner = *info.fields.start();
      switch (inner.typeInfo.typeRef) {
         case V_INT32ARRAY:
            info.header.flags |= elDebugDWORDS;
            break;
         case V_FLOAT64ARRAY:
            info.header.flags |= elDebugFLOAT64S;
            break;
         default:
            break;
      }
   }
   else if (isWrapper(info)) {
      auto inner = *info.fields.start();
      switch (inner.typeInfo.typeRef) {
         case V_INT32:
         case V_UINT32:
         case V_INT8:
         case V_UINT8:
         case V_PTR32:
         case V_WORD32:
         case V_INT16:
         case V_UINT16:
            info.header.flags |= elDebugDWORD;
            break;
         case V_INT64:
         case V_PTR64:
         case V_WORD64:
            info.header.flags |= elDebugQWORD;
            break;
         case V_FLOAT64:
            info.header.flags |= elDebugFLOAT64;
            break;
         case V_MESSAGE:
            info.header.flags |= elMessage;
            break;
         case V_MESSAGENAME:
            info.header.flags |= elMessageName;
            break;
         case V_INT32ARRAY:
            info.header.flags |= elDebugDWORDS;
            break;
         case V_FLOAT64ARRAY:
            info.header.flags |= elDebugFLOAT64S;
            break;
         default:
            break;
      }
   }
   else if ((info.header.flags & elDebugMask) == elInterface) {
      // verify if it is a weak interface (an interface without output type specified)
      bool isWeakInterface = true;
      for (auto it = info.methods.start(); !it.eof(); ++it) {
         auto methodInfo = *it;

         if (!methodInfo.inherited && methodInfo.outputRef && methodInfo.outputRef != classRef) {
            isWeakInterface = false;
            break;
         }
      }
      if (isWeakInterface) {
         info.header.flags &= ~elDebugMask;
         info.header.flags |= elWeakInterface;
      }
   }
}

void CompilerLogic :: tweakPrimitiveClassFlags(ClassInfo& info, ref_t classRef)
{

}

void CompilerLogic :: writeTypeMapEntry(MemoryBase* section, ustr_t key, ref_t reference)
{
   MemoryWriter writer(section);
   writer.writeString(key);
   writer.writeDWord(2);
   writer.writeRef(reference);
}

bool CompilerLogic :: readTypeMap(ModuleBase* extModule, MemoryBase* section, ReferenceMap& map, ModuleScopeBase* scope)
{
   IdentifierString key;

   MemoryReader reader(section);
   while (!reader.eof()) {
      reader.readString(key);
      int type = reader.getDWord();

      if (type == 2) {
         ref_t reference = reader.getRef();
         if (scope->module != extModule) {
            if (scope->isStandardOne()) {
               // HOTFIX : import predefined references
               ustr_t name = extModule->resolveReference(reference);
               if (NamespaceString::compareNs(name, scope->module->name())) {
                  reference = scope->module->mapReference(name + getlength(STANDARD_MODULE));
               }
               else reference = ImportHelper::importReference(extModule, reference, scope->module);
            }
            else reference = ImportHelper::importReference(extModule, reference, scope->module);
         }

         map.add(*key, reference);
      }
      else return false;
   }

   return true;
}

//void CompilerLogic :: writeDeclDictionaryEntry(MemoryBase* section, ustr_t key, ref_t reference)
//{
//   MemoryWriter writer(section);
//   writer.writeString(key);
//   writer.writeDWord(3);
//   writer.writeRef(reference);
//}
//
//bool CompilerLogic :: readDeclDictionary(ModuleBase* extModule, MemoryBase* section, ReferenceMap& map, ModuleScopeBase* scope)
//{
//   IdentifierString key;
//
//   MemoryReader reader(section);
//   while (!reader.eof()) {
//      reader.readString(key);
//      int type = reader.getDWord();
//
//      if (type == 3) {
//         ref_t reference = reader.getRef();
//         if (scope->module != extModule) {
//            reference = scope->importReference(extModule, reference);
//         }
//
//         map.add(*key, reference);
//      }
//      else return false;
//   }
//
//   return true;
//}

void CompilerLogic :: writeAttributeMapEntry(MemoryBase* section, ustr_t key, int value)
{
   MemoryWriter writer(section);
   writer.writeString(key);
   writer.writeDWord(1);
   writer.writeDWord(value);
}

void CompilerLogic :: writeAttributeMapEntry(MemoryBase* section, ustr_t key, ustr_t value)
{
   MemoryWriter writer(section);
   writer.writeString(key);
   writer.writeDWord(2);
   writer.writeDWord(value.length_pos());
   writer.writeString(value);
}

bool CompilerLogic :: readAttributeMap(MemoryBase* section, ReferenceMap& map)
{
   IdentifierString key;

   MemoryReader reader(section);
   while (!reader.eof()) {
      reader.readString(key);
      int type = reader.getDWord();

      if (type == 1) {
         ref_t value = reader.getRef();

         map.add(*key, value);
      }
      else return false;
   }

   return true;
}

void CompilerLogic :: writeArrayEntry(MemoryBase* section, ref_t reference)
{
   MemoryWriter writer(section);
   writer.writeRef(reference);
}

void CompilerLogic :: writeArrayReference(MemoryBase* section, ref_t reference)
{
   MemoryWriter writer(section);
   writer.writeDReference(reference, 0);
}

void CompilerLogic :: writeExtMessageEntry(MemoryBase* section, ref_t extRef, mssg_t message, mssg_t strongMessage)
{
   MemoryWriter writer(section);
   writer.writeRef(extRef);
   writer.writeRef(message);
   writer.writeRef(strongMessage);
}

void CompilerLogic :: writeExtMessageEntry(MemoryBase* section, mssg_t message, ustr_t pattern)
{
   MemoryWriter writer(section);
   writer.writeRef(0);
   writer.writeRef(message);
   writer.writeRef(0);
   writer.writeString(pattern);
}

bool CompilerLogic :: readExtMessageEntry(ModuleBase* extModule, MemoryBase* section, ExtensionMap& map,
   ExtensionTemplateMap& extensionTemplates, ModuleScopeBase* scope)
{
   bool importMode = extModule != scope->module;

   IdentifierString key;

   MemoryReader reader(section);
   while (!reader.eof()) {
      ref_t extRef = reader.getRef();
      if (importMode && extRef)
         extRef = ImportHelper::importReference(extModule, extRef, scope->module);

      mssg_t message = reader.getRef();
      if (importMode)
         message = ImportHelper::importMessage(extModule, message, scope->module);

      mssg_t strongMessage = reader.getRef();
      if (importMode && strongMessage)
         strongMessage = ImportHelper::importMessage(extModule, strongMessage, scope->module);

      if (!extRef) {
         // if it is an extension template
         ustr_t pattern = reader.getString(DEFAULT_STR);

         extensionTemplates.add(message, pattern.clone());
      }
      else map.add(message, { extRef, strongMessage });
   }

   return true;
}

bool CompilerLogic :: defineClassInfo(ModuleScopeBase& scope, ClassInfo& info, ref_t reference,
   bool headerOnly, bool fieldsOnly)
{
   if (isPrimitiveRef(reference) && !headerOnly) {
      scope.loadClassInfo(info, scope.buildins.superReference);
   }

   switch (reference)
   {
      case V_INT64:
      case V_PTR64:
      case V_WORD64:
         info.header.parentRef = scope.buildins.superReference;
         info.header.flags = elDebugQWORD | elStructureRole | elReadOnlyRole;
         info.size = 8;
         break;
      case V_FLOAT64:
         info.header.parentRef = scope.buildins.superReference;
         info.header.flags = elDebugFLOAT64 | elStructureRole | elReadOnlyRole;
         info.size = 8;
         break;
      case V_INT32:
      case V_UINT32:
      case V_PTR32:
      case V_WORD32:
         info.header.parentRef = scope.buildins.superReference;
         info.header.flags = elDebugDWORD | elStructureRole | elReadOnlyRole;
         info.size = 4;
         break;
      case V_INT8:
      case V_UINT8:
         info.header.parentRef = scope.buildins.superReference;
         info.header.flags = elDebugDWORD | elStructureRole | elReadOnlyRole;
         info.size = 1;
         break;
      case V_INT16:
      case V_UINT16:
         info.header.parentRef = scope.buildins.superReference;
         info.header.flags = elDebugDWORD | elStructureRole | elReadOnlyRole;
         info.size = 2;
         break;
      case V_INT8ARRAY:
         info.header.parentRef = scope.buildins.superReference;
         info.header.flags = elDebugBytes | elStructureRole | elDynamicRole | elWrapper;
         info.size = -1;
         break;
      case V_INT16ARRAY:
         info.header.parentRef = scope.buildins.superReference;
         info.header.flags = /*elDebugBytes | */elStructureRole | elDynamicRole | elWrapper;
         info.size = -2;
         break;
      case V_INT32ARRAY:
         info.header.parentRef = scope.buildins.superReference;
         info.header.flags = elDebugDWORDS | elStructureRole | elDynamicRole | elWrapper;
         info.size = -4;
         break;
      case V_FLOAT64ARRAY:
         info.header.parentRef = scope.buildins.superReference;
         info.header.flags = elDebugFLOAT64S | elStructureRole | elDynamicRole | elWrapper;
         info.size = -8;
         break;
      case V_BINARYARRAY:
         info.header.parentRef = scope.buildins.superReference;
         info.header.flags = /*elDebugBytes | */elStructureRole | elDynamicRole | elWrapper;
         info.size = -1;
         break;
      case V_OBJARRAY:
         info.header.parentRef = scope.buildins.superReference;
         info.header.flags = /*elDebugArray | */elDynamicRole;
         info.size = 0;
         break;
      case V_AUTO:
         break;
      default:
         if (reference != 0) {
            if (!scope.loadClassInfo(info, reference, headerOnly, fieldsOnly))
               return false;
         }
         else {
            info.header.parentRef = 0;
            info.header.flags = 0;
            info.size = 0;
         }
         break;
   }

   return true;
}

SizeInfo CompilerLogic :: defineStructSize(ClassInfo& info)
{
   SizeInfo sizeInfo = { 0, test(info.header.flags, elReadOnlyRole) };

   if (isEmbeddableStruct(info)) {
      sizeInfo.size = info.size;

      return sizeInfo;
   }
   else if (isEmbeddableArray(info)) {
      sizeInfo.size = info.size;

      return sizeInfo;
   }

   return {};
}

SizeInfo CompilerLogic :: defineStructSize(ModuleScopeBase& scope, ref_t reference)
{
   if (!reference)
      return {};

   auto sizeInfo = scope.cachedSizes.get(reference);
   if (!sizeInfo.size) {
      ClassInfo classInfo;
      if (defineClassInfo(scope, classInfo, reference)) {
         sizeInfo = defineStructSize(classInfo);

         if (sizeInfo.size)
            scope.cachedSizes.add(reference, sizeInfo);

         return sizeInfo;
      }
      else return { 0 };
   }
   else return sizeInfo;
}

ref_t CompilerLogic :: definePrimitiveArray(ModuleScopeBase& scope, ref_t elementRef, bool structOne)
{
   ClassInfo info;
   if (!defineClassInfo(scope, info, elementRef, true))
      return 0;

   if (isEmbeddableStruct(info) && structOne) {
      if (isCompatible(scope, { V_INT8 }, { elementRef }, true) && info.size == 1)
         return V_INT8ARRAY;

      if (isCompatible(scope, { V_INT16 }, { elementRef }, true) && info.size == 2)
         return V_INT16ARRAY;

      if (isCompatible(scope, { V_INT32 }, { elementRef }, true) && info.size == 4)
         return V_INT32ARRAY;

      if (isCompatible(scope, { V_FLOAT64 }, { elementRef }, true) && info.size == 8)
         return V_FLOAT64ARRAY;

      return V_BINARYARRAY;
   }
   else return V_OBJARRAY;
}

bool CompilerLogic :: isTemplateCompatible(ModuleScopeBase& scope, ref_t targetRef, ref_t sourceRef, bool weakCompatible)
{
   ustr_t targetName = scope.module->resolveReference(targetRef);
   ustr_t sourceName = scope.module->resolveReference(sourceRef);

   size_t pos = targetName.find('&');

   // check if it is the same template
   if (sourceName.length() < pos || !targetName.compare(sourceName, pos))
      return false;

   if (weakCompatible)
      return true;

   // check if the signature is compatible
   size_t targetStart = pos + 1;
   size_t sourceStart = pos + 1;

   while (true) {
      size_t targetEndPos = targetName.findSub(targetStart, '&', targetName.length());
      size_t sourceEndPos = sourceName.findSub(sourceStart, '&', sourceName.length());

      IdentifierString targetArg;
      IdentifierString sourceArg;

      targetArg.copy(targetName.str() + targetStart, targetEndPos - targetStart);
      sourceArg.copy(sourceName.str() + sourceStart, sourceEndPos - sourceStart);

      targetArg.replaceAll('@', '\'', 0);
      sourceArg.replaceAll('@', '\'', 0);

      ref_t targetArgRef = scope.mapFullReference(*targetArg);
      ref_t sourceArgRef = scope.mapFullReference(*sourceArg);

      if (!isCompatible(scope, { targetArgRef }, { sourceArgRef }, true))
         return false;

      if (targetEndPos >= targetName.length() || sourceEndPos >= sourceName.length())
         break;

      targetStart = targetEndPos + 1;
      sourceStart = sourceEndPos + 1;
   }

   return true;
}

bool CompilerLogic :: isCompatible(ModuleScopeBase& scope, TypeInfo targetInfo, TypeInfo sourceInfo, bool ignoreNils)
{
   if ((!targetInfo.typeRef || targetInfo.typeRef == scope.buildins.superReference) && !sourceInfo.isPrimitive())
      return true;

   switch (sourceInfo.typeRef) {
      case V_NIL:
         // nil is compatible with a super class for the message dispatching
         // and with all types for all other cases
         if (!ignoreNils || targetInfo.typeRef == scope.buildins.superReference)
            return true;

         break;
      case V_STRING:
         if (targetInfo == sourceInfo) {
            return true;
         }
         else return isCompatible(scope, targetInfo, { scope.buildins.literalReference }, ignoreNils);
         break;
      case V_WIDESTRING:
         if (targetInfo == sourceInfo) {
            return true;
         }
         else return isCompatible(scope, targetInfo, { scope.buildins.wideReference }, ignoreNils);
         break;
      case V_FLAG:
         return isCompatible(scope, targetInfo, { scope.branchingInfo.typeRef }, ignoreNils);
      default:
         break;
   }

   if (targetInfo.isPrimitive() && isPrimitiveCompatible(scope, targetInfo, sourceInfo))
      return true;

   while (sourceInfo.typeRef != 0) {
      if (targetInfo != sourceInfo) {
         ClassInfo info;
         if (sourceInfo.isPrimitive() || !defineClassInfo(scope, info, sourceInfo.typeRef))
            return false;

         if (test(info.header.flags, elTemplatebased) && !isPrimitiveRef(targetInfo.typeRef)) {
            // HOTFIX : resolve weak reference before checking compability
            targetInfo.typeRef = scope.resolveWeakTemplateReferenceID(targetInfo.typeRef);
            info.header.parentRef = scope.resolveWeakTemplateReferenceID(info.header.parentRef);
            if (targetInfo == sourceInfo) {
               return true;
            }
         }

         // if it is a structure wrapper
         if (targetInfo.isPrimitive() && test(info.header.flags, elWrapper)) {
            if (info.fields.count() > 0) {
               auto inner = *info.fields.start();
               if (isCompatible(scope, targetInfo, inner.typeInfo, ignoreNils))
                  return true;
            }
         }

         if (test(info.header.flags, elClassClass)) {
            // class class can be compatible only with itself and the super class
            sourceInfo = { scope.buildins.superReference };
         }
         else sourceInfo = { info.header.parentRef };
      }
      else return true;
   }

   return false;
}

inline ref_t getSignature(ModuleScopeBase& scope, mssg_t message)
{
   ref_t actionRef = getAction(message);
   ref_t signRef = 0;
   scope.module->resolveAction(actionRef, signRef);

   return signRef;
}

bool CompilerLogic :: isSignatureCompatible(ModuleScopeBase& scope, ModuleBase* targetModule, ref_t targetSignature,
   ref_t* sourceSignatures, size_t sourceLen)
{
   ref_t targetSignatures[ARG_COUNT];
   size_t len = targetModule->resolveSignature(targetSignature, targetSignatures);

   if (sourceLen == 0 && len == 0)
      return true;

   if (len < 1)
      return false;

   for (size_t i = 0; i < sourceLen; i++) {
      ref_t targetSign = i < len ? targetSignatures[i] : targetSignatures[len - 1];

      if (!isCompatible(scope, { ImportHelper::importReference(targetModule, targetSign, scope.module) }, { sourceSignatures[i] }, true))
         return false;
   }

   return true;
}

bool CompilerLogic :: isSignatureCompatible(ModuleScopeBase& scope, ref_t targetSignature,
   ref_t* sourceSignatures, size_t sourceLen)
{
   ref_t targetSignatures[ARG_COUNT];
   size_t len = scope.module->resolveSignature(targetSignature, targetSignatures);
   if (sourceLen == 0 && len == 0)
      return true;

   if (len < 1)
      return false;

   for (size_t i = 0; i < sourceLen; i++) {
      ref_t targetSign = i < len ? targetSignatures[i] : targetSignatures[len - 1];
      if (!isCompatible(scope, { targetSign }, { sourceSignatures[i] }, true))
         return false;
   }

   return true;
}

bool CompilerLogic :: isSignatureCompatible(ModuleScopeBase& scope, mssg_t targetMessage, mssg_t sourceMessage)
{
   ref_t sourceSignatures[ARG_COUNT];
   size_t len = scope.module->resolveSignature(getSignature(scope, sourceMessage), sourceSignatures);

   return isSignatureCompatible(scope, getSignature(scope, targetMessage), sourceSignatures, len);
}

bool CompilerLogic :: isMessageCompatibleWithSignature(ModuleScopeBase& scope, ref_t targetRef,
   mssg_t targetMessage, ref_t* sourceSignature, size_t len, int& stackSafeAttr)
{
   ref_t targetSignRef = getSignature(scope, targetMessage);

   if (isSignatureCompatible(scope, targetSignRef, sourceSignature, len)) {
      if (isStacksafeArg(scope, targetRef))
         stackSafeAttr |= 1;

      setSignatureStacksafe(scope, targetSignRef, stackSafeAttr);

      return true;
   }
   else return false;
}

ref_t CompilerLogic :: getClassClassRef(ModuleScopeBase& scope, ref_t targetRef)
{
   ClassInfo info;
   if (!defineClassInfo(scope, info, targetRef, true))
      return 0;

   return info.header.classRef;
}

void CompilerLogic :: setSignatureStacksafe(ModuleScopeBase& scope, ref_t targetSignature, int& stackSafeAttr)
{
   ref_t targetSignatures[ARG_COUNT];
   size_t len = scope.module->resolveSignature(targetSignature, targetSignatures);
   if (len <= 0)
      return;

   int flag = 1;
   for (size_t i = 0; i < len; i++) {
      flag <<= 1;

      if (isStacksafeArg(scope, targetSignatures[i]))
         stackSafeAttr |= flag;
   }
}

void CompilerLogic :: setSignatureStacksafe(ModuleScopeBase& scope, ModuleBase* targetModule,
   ref_t targetSignature, int& stackSafeAttr)
{
   ref_t targetSignatures[ARG_COUNT];
   size_t len = targetModule->resolveSignature(targetSignature, targetSignatures);
   if (len <= 0)
      return;

   int flag = 1;
   for (size_t i = 0; i < len; i++) {
      flag <<= 1;

      if (isStacksafeArg(scope, ImportHelper::importReference(targetModule, targetSignatures[i], scope.module)))
         stackSafeAttr |= flag;
   }
}

inline mssg_t resolveNonpublic(mssg_t weakMessage, ClassInfo& info, bool selfCall, bool isInternalOp)
{
   ref_t nonpublicMessage = 0;
   if (selfCall && !test(weakMessage, STATIC_MESSAGE) && info.methods.exist(weakMessage | STATIC_MESSAGE)) {
      nonpublicMessage = weakMessage | STATIC_MESSAGE;
   }
   else {
      mssg_t protectedMessage = info.attributes.get({ weakMessage, ClassAttribute::ProtectedAlias });
      if (protectedMessage && selfCall) {
         nonpublicMessage = protectedMessage;
      }
      else {
         mssg_t internalMessage = info.attributes.get({ weakMessage, ClassAttribute::InternalAlias });
         if (internalMessage && isInternalOp) {
            nonpublicMessage = internalMessage;
         }
      }
   }

   return nonpublicMessage;
}

inline ref_t mapWeakSignature(ModuleScopeBase& scope, int counter)
{
   ref_t signatures[ARG_COUNT] = { 0 };
   ref_t signatureLen = counter;
   for (int i = 0; i < counter; i++)
      signatures[i] = scope.buildins.superReference;

   return scope.module->mapSignature(signatures, signatureLen, false);
}

mssg_t CompilerLogic :: resolveMultimethod(ModuleScopeBase& scope, mssg_t weakMessage, ref_t targetRef,
   ref_t implicitSignatureRef, int& stackSafeAttr, bool selfCall)
{
   if (!targetRef)
      return 0;

   ClassInfo info;
   if (defineClassInfo(scope, info, targetRef)) {
      if (isStacksafeArg(info))
         stackSafeAttr |= 1;

      // check if it is non public message
      mssg_t nonPublicMultiMessage = resolveNonpublic(weakMessage, info, selfCall, isInternalOp(scope, targetRef));
      if (nonPublicMultiMessage != 0) {
         if (!implicitSignatureRef && test(nonPublicMultiMessage, STATIC_MESSAGE) && getArgCount(weakMessage) > 1) {
            implicitSignatureRef = mapWeakSignature(scope, getArgCount(weakMessage) - 1);
         }

         mssg_t resolved = resolveMultimethod(scope, nonPublicMultiMessage, targetRef, implicitSignatureRef, stackSafeAttr, selfCall);
         if (!resolved) {
            return nonPublicMultiMessage;
         }
         else return resolved;
      }

      // allow to check the variadic message without arguments
      if (!implicitSignatureRef && ((weakMessage & PREFIX_MESSAGE_MASK) != VARIADIC_MESSAGE))
         return 0;

      if (!implicitSignatureRef) {
         implicitSignatureRef = mapWeakSignature(scope, getArgCount(weakMessage));
      }

      ref_t signatures[ARG_COUNT];
      size_t signatureLen = scope.module->resolveSignature(implicitSignatureRef, signatures);

      ref_t listRef = info.attributes.get({ weakMessage, ClassAttribute::OverloadList });
      if (listRef) {
         auto sectionInfo = scope.getSection(scope.module->resolveReference(listRef), mskConstArray, true);
         if (!sectionInfo.section || sectionInfo.section->length() < 4)
            return 0;

         MemoryReader reader(sectionInfo.section);
         pos_t position = sectionInfo.section->length() - 4;
         mssg_t foundMessage = 0;
         while (position != 0) {
            reader.seek(position - 8);
            mssg_t argMessage = reader.getRef();
            ref_t argSign = 0;
            sectionInfo.module->resolveAction(getAction(argMessage), argSign);

            if (sectionInfo.module == scope.module) {
               if (isSignatureCompatible(scope, argSign, signatures, signatureLen)) {
                  setSignatureStacksafe(scope, argSign, stackSafeAttr);

                  foundMessage = argMessage;
               }
            }
            else {
               if (isSignatureCompatible(scope, sectionInfo.module,
                  argSign, signatures, signatureLen))
               {
                  setSignatureStacksafe(scope, sectionInfo.module, argSign, stackSafeAttr);

                  foundMessage = ImportHelper::importMessage(sectionInfo.module, argMessage, scope.module);
               }
            }

            position -= 8;
         }
         return foundMessage;
      }
   }

   return 0;
}

mssg_t CompilerLogic :: retrieveDynamicConvertor(ModuleScopeBase& scope, ref_t targetRef)
{
   ref_t classClassRef = getClassClassRef(scope, targetRef);
   mssg_t messageRef = overwriteArgCount(scope.buildins.constructor_message, 1);

   CheckMethodResult dummy = {};
   if (checkMethod(scope, classClassRef, messageRef, dummy)) {
      return messageRef;
   }

   return 0;
}

mssg_t CompilerLogic :: retrieveImplicitConstructor(ModuleScopeBase& scope, ref_t targetRef, ref_t signRef,
   pos_t signLen, int& stackSafeAttrs)
{
   ref_t classClassRef = getClassClassRef(scope, targetRef);
   mssg_t messageRef = overwriteArgCount(scope.buildins.constructor_message, signLen);

   // try to resolve implicit multi-method
   mssg_t resolvedMessage = resolveMultimethod(scope, messageRef, classClassRef,
      signRef, stackSafeAttrs, false);

   if (resolvedMessage)
      return resolvedMessage;

   stackSafeAttrs = 0;

   return 0;
}

ConversionRoutine CompilerLogic :: retrieveConversionRoutine(CompilerBase* compiler, ModuleScopeBase& scope, ustr_t ns,
   ref_t targetRef, TypeInfo sourceInfo, bool directConversion)
{
   ClassInfo info;
   if (!defineClassInfo(scope, info, targetRef))
      return { };

   // if the target class is wrapper around the source
   if (test(info.header.flags, elWrapper) && !test(info.header.flags, elDynamicRole)) {
      auto inner = *info.fields.start();

      bool compatible = false;
      compatible = isCompatible(scope, inner.typeInfo, sourceInfo, false);

      if (compatible)
         return { ConversionResult::BoxingRequired };

      if (inner.typeInfo.typeRef == V_INT32 && isCompatible(scope, { V_UINT8 }, sourceInfo, false)) {
         return { ConversionResult::NativeConversion, INT8_32_CONVERSION, 1 };
      }
      if (inner.typeInfo.typeRef == V_INT32 && isCompatible(scope, { V_INT8 }, sourceInfo, false)) {
         return { ConversionResult::NativeConversion, INT8_32_CONVERSION, 1 };
      }
      if (inner.typeInfo.typeRef == V_INT32 && isCompatible(scope, { V_INT16 }, sourceInfo, false)) {
         return { ConversionResult::NativeConversion, INT16_32_CONVERSION, 1 };
      }
      if (inner.typeInfo.typeRef == V_INT64 && isCompatible(scope, { V_INT32 }, sourceInfo, false)) {
         return { ConversionResult::NativeConversion, INT32_64_CONVERSION, 1 };
      }
      if (inner.typeInfo.typeRef == V_FLOAT64 && isCompatible(scope, { V_INT32 }, sourceInfo, false)) {
         return { ConversionResult::NativeConversion, INT32_FLOAT64_CONVERSION, 1 };
      }
   }

   // COMPILE MAGIC : trying to typecast primitive array
   if (isPrimitiveArrRef(sourceInfo.typeRef) && test(info.header.flags, elDynamicRole)) {
      auto inner = *info.fields.start();

      bool compatible = isCompatible(scope, { inner.typeInfo.elementRef }, { sourceInfo.elementRef }, false);
      if (compatible)
         return { ConversionResult::BoxingRequired };
   }
   // COMPILE MAGIC : trying to typecast variadic array
   else if (sourceInfo.typeRef == V_ARGARRAY && test(info.header.flags, elDynamicRole)) {
      auto inner = *info.fields.start();

      bool compatible = isCompatible(scope, { inner.typeInfo.elementRef }, { sourceInfo.elementRef }, false);
      if (compatible)
         return { ConversionResult::VariadicBoxingRequired };
   }

   // if there is a implicit conversion routine
   if (!isPrimitiveRef(targetRef) && !directConversion) {
      ref_t sourceRef = sourceInfo.isPrimitive() ? compiler->resolvePrimitiveType(scope, sourceInfo) : sourceInfo.typeRef;

      ref_t signRef = scope.module->mapSignature(&sourceRef, 1, false);
      int stackSafeAttrs = 0;
      mssg_t messageRef = retrieveImplicitConstructor(scope, targetRef, signRef, 1, stackSafeAttrs);
      if (messageRef)
         return { ConversionResult::Conversion, messageRef, stackSafeAttrs };

      if (!sourceRef || sourceRef == scope.buildins.superReference) {
         // if it is a weak argument / check dynamic convertor
         mssg_t messageRef = retrieveDynamicConvertor(scope, targetRef);
         if (messageRef)
            return { ConversionResult::DynamicConversion, messageRef, stackSafeAttrs };
      }

   }

   return {};
}

bool CompilerLogic :: checkMethod(ClassInfo& info, mssg_t message, CheckMethodResult& result)
{
   bool methodFound = info.methods.exist(message);
   if (methodFound) {
      MethodInfo methodInfo = info.methods.get(message);

      result.message = message;
      result.outputInfo = { methodInfo.outputRef };
      if (MethodInfo::checkVisibility(methodInfo, MethodHint::Private)) {
         result.visibility = Visibility::Private;
      }
      else if (MethodInfo::checkVisibility(methodInfo, MethodHint::Protected)) {
         result.visibility = Visibility::Protected;
      }
      else if (MethodInfo::checkVisibility(methodInfo, MethodHint::Internal)) {
         result.visibility = Visibility::Internal;
      }
      else result.visibility = Visibility::Public;

      // check nillable attribute
      result.outputInfo.nillable = test(methodInfo.hints, (ref_t)MethodHint::Nillable);

      result.kind = methodInfo.hints & (ref_t)MethodHint::Mask;
      if (result.kind == (ref_t)MethodHint::Normal) {
         // check if the normal method can be called directly / semi-directly
         if (test(info.header.flags, elSealed)) {
            result.kind = (ref_t)MethodHint::Sealed; // mark it as sealed - because the class is sealed
         }
         else if (MethodInfo::checkHint(methodInfo, MethodHint::Indexed)) {
            result.kind = (ref_t)MethodHint::ByIndex;
         }
         else if (test(info.header.flags, elClosed)) {
            result.kind = (ref_t)MethodHint::Fixed; // mark it as fixed - because the class is closed
         }
      }

      result.stackSafe = test(methodInfo.hints, (ref_t)MethodHint::Stacksafe);
      result.nillableArgs = methodInfo.nillableArgs;
      result.byRefHandler = methodInfo.byRefHandler;

      if (test(methodInfo.hints, (ref_t)MethodHint::Constant)) {
         result.constRef = info.attributes.get({ message, ClassAttribute::ConstantMethod });
      }

      if (test(methodInfo.hints, (ref_t)MethodHint::Initializer)) {
         result.kind = (ref_t)MethodHint::Sealed;
      }

      return true;
   }
   else return false;
}

bool CompilerLogic :: checkMethod(ModuleScopeBase& scope, ref_t classRef, mssg_t message, CheckMethodResult& result)
{
   ClassInfo info;
   if (classRef && defineClassInfo(scope, info, classRef)) {
      if (testany(info.header.flags, elWithVariadics)) {
         result.withVariadicDispatcher = true;
      }
      else if (test(info.header.flags, elWithCustomDispatcher))
         result.withCustomDispatcher = true;

      return checkMethod(info, message, result);
   }
   else return false;
}

mssg_t CompilerLogic :: retrieveByRefHandler(ModuleScopeBase& scope, ref_t reference, mssg_t message)
{
   CheckMethodResult result = {};
   if (checkMethod(scope, reference, message, result)) {
      return result.byRefHandler;
   }

   return 0;
}

bool CompilerLogic :: isMessageSupported(ClassInfo& info, mssg_t message)
{
   CheckMethodResult dummy = {};

   return isMessageSupported(info, message, dummy);
}

bool CompilerLogic :: isMessageSupported(ClassInfo& info, mssg_t message, CheckMethodResult& result)
{
   if (!checkMethod(info, message, result)) {
      if (checkMethod(info, message | STATIC_MESSAGE, result)) {
         if (result.visibility == Visibility::Private)
            return true;

         result = {};
      }
      mssg_t protectedMessage = info.attributes.get({ message, ClassAttribute::ProtectedAlias });
      if (protectedMessage) {
         if (checkMethod(info, protectedMessage, result)) {
            result.visibility = Visibility::Protected;
            return true;
         }
      }
      mssg_t internalMessage = info.attributes.get({ message, ClassAttribute::InternalAlias });
      if (internalMessage) {
         if (checkMethod(info, internalMessage, result)) {
            result.visibility = Visibility::Internal;
            return true;
         }
      }
   }
   else return true;

   return false;
}

bool CompilerLogic :: resolveCallType(ModuleScopeBase& scope, ref_t classRef, mssg_t message,
   CheckMethodResult& result)
{
   if (!classRef)
      classRef = scope.buildins.superReference;

   ClassInfo info;
   if (defineClassInfo(scope, info, classRef)) {
      if (testany(info.header.flags, elWithVariadics)) {
         result.withVariadicDispatcher = true;
      }
      else if (test(info.header.flags, elWithCustomDispatcher))
         result.withCustomDispatcher = true;

      return isMessageSupported(info, message, result);
   }

   return false;
}

bool CompilerLogic :: isNeedVerification(ClassInfo& info, VirtualMethodList& implicitMultimethods)
{
   // HOTFIX : Make sure the multi-method methods have the same output type as generic one
   for (auto it = implicitMultimethods.start(); !it.eof(); it++) {
      auto vm = *it;

      mssg_t message = vm.message;

      auto methodInfo = info.methods.get(message);
      ref_t outputRef = methodInfo.outputRef;
      if (outputRef != 0) {
         // Bad luck we have to verify all overloaded methods
         return true;
      }
   }

   return false;
}

bool CompilerLogic :: verifyMultimethod(ModuleScopeBase& scope, ClassInfo& info, mssg_t message)
{
   auto methodInfo = info.methods.get(message);
   if (methodInfo.multiMethod != 0) {
      auto multiMethodInfo = info.methods.get(methodInfo.multiMethod);
      ref_t outputRefMulti = multiMethodInfo.outputRef;
      if (outputRefMulti != 0) {
         ref_t outputRef = methodInfo.outputRef;
         if (outputRef == 0) {
            return false;
         }
         else if (!isCompatible(scope, { outputRefMulti }, { outputRef }, true)) {
            return false;
         }
      }
   }

   return true;
}

inline ustr_t resolveActionName(ModuleBase* module, mssg_t message)
{
   ref_t signRef = 0;
   return module->resolveAction(getAction(message), signRef);
}

ref_t CompilerLogic :: generateOverloadList(CompilerBase* compiler, ModuleScopeBase& scope, MethodHint callType, ClassInfo::MethodMap& methods,
   mssg_t message, void* param, ref_t(*resolve)(void*, ref_t))
{
   // create a new overload list
   ref_t listRef = scope.mapAnonymous(resolveActionName(scope.module, message));

   // sort the overloadlist
   CachedList<mssg_t, 0x20> list;
   for (auto m_it = methods.start(); !m_it.eof(); ++m_it) {
      auto methodInfo = *m_it;
      if (methodInfo.multiMethod == message) {
         bool added = false;
         mssg_t omsg = m_it.key();
         pos_t len = list.count_pos();
         for (pos_t i = 0; i < len; i++) {
            if (isSignatureCompatible(scope, omsg, list[i])) {
               list.insert(i, omsg);
               added = true;
               break;
            }
         }
         if (!added)
            list.add(omsg);
      }
   }

   // fill the overloadlist
   for (size_t i = 0; i < list.count(); i++) {
      ref_t classRef = resolve(param, list[i]);

      compiler->generateOverloadListMember(scope, listRef, classRef, list[i], callType);
   }

   return listRef;
}

ref_t paramFeedback(void* param, ref_t)
{
#if defined(__LP64__)
   size_t val = (size_t)param;

   return (ref_t)val;
#else
   return ptrToUInt32(param);
#endif
}

void CompilerLogic :: injectMethodOverloadList(CompilerBase* compiler, ModuleScopeBase& scope, MethodHint callType,
   mssg_t message, ClassInfo::MethodMap& methods, ClassAttributes& attributes,
   void* param, ref_t(*resolve)(void*, ref_t), ClassAttribute attribute)
{
   ref_t listRef = generateOverloadList(compiler, scope, callType, methods, message, param, resolve);

   ClassAttributeKey key = { message, attribute };
   attributes.exclude(key);
   attributes.add(key, listRef);
}

void CompilerLogic :: injectOverloadList(CompilerBase* compiler, ModuleScopeBase& scope, ClassInfo& info, ref_t classRef)
{
   for (auto it = info.methods.start(); !it.eof(); ++it) {
      auto methodInfo = *it;
      if (!methodInfo.inherited && isMultiMethod(info, methodInfo)) {
         // create a new overload list
         mssg_t message = it.key();

         MethodHint callType = MethodHint::Normal;
         if (test(info.header.flags, elSealed) || MethodInfo::checkHint(methodInfo, MethodHint::Sealed)) {
            callType = MethodHint::Sealed;
         }
         else if (MethodInfo::checkHint(methodInfo, MethodHint::Indexed)) {
            callType = MethodHint::ByIndex;
         }
         else if (test(message, STATIC_MESSAGE)) {
            // NOTE : the check must be after the checking of indexed method to correctly deal with hidden indexed methods
            callType = MethodHint::Sealed;
         }
         else if (test(info.header.flags, elClosed)) {
            callType = MethodHint::Fixed;
         }

         injectMethodOverloadList(compiler, scope, callType, message,
            info.methods, info.attributes, UInt32ToPtr(classRef), paramFeedback, ClassAttribute::OverloadList);
      }
   }
}

bool CompilerLogic :: isValidType(ModuleScopeBase& scope, ref_t classReference, bool ignoreUndeclared)
{
   ClassInfo info;
   if (!defineClassInfo(scope, info, classReference, true))
      return ignoreUndeclared;

   return true;
}

void CompilerLogic :: generateVirtualDispatchMethod(ModuleScopeBase& scope, ref_t parentRef, VirtualMethods& methods)
{
   ClassInfo info;
   scope.loadClassInfo(info, parentRef);
   for (auto it = info.methods.start(); !it.eof(); ++it) {
      auto mssg = it.key();
      auto methodInfo = *it;

      if (test(methodInfo.hints, (ref_t)MethodHint::Abstract)) {
         methods.add({ mssg, methodInfo.outputRef });
      }
   }
}

mssg_t CompilerLogic :: resolveSingleDispatch(ModuleScopeBase& scope, ref_t reference, ref_t weakMessage, bool selfCall, int& nillableArgs)
{
   if (!reference)
      return 0;

   ClassInfo info;
   if (defineClassInfo(scope, info, reference)) {
      mssg_t dispatcher = info.attributes.get({ weakMessage, ClassAttribute::SingleDispatch });
      if (!dispatcher) {
         // check if it is non public message
         mssg_t nonPublicWeakMessage = resolveNonpublic(weakMessage, info, selfCall, isInternalOp(scope, reference));
         if (nonPublicWeakMessage != 0) {
            dispatcher = info.attributes.get({ nonPublicWeakMessage, ClassAttribute::SingleDispatch });
         }
      }

      if (dispatcher) {
         CheckMethodResult result;
         if (checkMethod(info, dispatcher, result))
            nillableArgs = result.nillableArgs;
      }

      return dispatcher;
   }
   else return 0;
}

mssg_t CompilerLogic :: resolveFunctionSingleDispatch(ModuleScopeBase& scope, ref_t reference, int& nillableArgs)
{
   if (!reference)
      return 0;

   ClassInfo info;
   if (defineClassInfo(scope, info, reference)) {
      ref_t actionRef = scope.module->mapAction(INVOKE_MESSAGE, 0, true);
      for (pos_t i = 0; i < ARG_COUNT; i++) {
         mssg_t weakMessage = encodeMessage(actionRef, i, FUNCTION_MESSAGE);

         if (info.methods.exist(weakMessage))
            return weakMessage;
      }
   }
   return 0;
}

inline size_t readSignatureMember(ustr_t signature, size_t index)
{
   int level = 0;
   size_t len = getlength(signature);
   for (size_t i = index; i < len; i++) {
      if (signature[i] == '&') {
         if (level == 0) {
            return i;
         }
         else level--;
      }
      else if (signature[i] == '#') {
         String<char, 5> tmp;
         size_t numEnd = signature.findSub(i, '&', NOTFOUND_POS);
         tmp.copy(signature.str() + i + 1, numEnd - i - 1);
         level += tmp.toInt();
      }
   }

   return len;
}

inline void decodeClassName(IdentifierString& signature)
{
   ustr_t ident = *signature;

   if (ident.startsWith(TEMPLATE_PREFIX_NS_ENCODED)) {
      // if it is encodeded weak reference - decode only the prefix
      signature[0] = '\'';
      signature[strlen(TEMPLATE_PREFIX_NS_ENCODED) - 1] = '\'';
   }
   else if (ident.startsWith(TEMPLATE_PREFIX_NS)) {
      // if it is weak reference - do nothing
   }
   else signature.replaceAll('@', '\'', 0);
}

ref_t CompilerLogic :: resolveExtensionTemplate(ModuleScopeBase& scope, CompilerBase* compiler, ustr_t pattern, ref_t signatureRef,
   ustr_t ns, ExtensionMap* outerExtensionList)
{
   size_t argumentLen = 0;
   ref_t parameters[ARG_COUNT] = { 0 };
   ref_t signatures[ARG_COUNT];
   scope.module->resolveSignature(signatureRef, signatures);

   // matching pattern with the provided signature
   size_t i = pattern.find('.') ;

   // define an argument length
   size_t argLenPos = pattern.findSub(0, '#', i, NOTFOUND_POS);
   if (i != NOTFOUND_POS) {
      i += 2;

      String<char, 5> tmp;
      tmp.copy(pattern + argLenPos + 1, i - argLenPos - 3);

      argumentLen = tmp.toInt();
   }
   else i = getlength(pattern);

   IdentifierString templateName(pattern, i - 2);
   ref_t templateRef = scope.mapFullReference(*templateName, true);

   size_t len = getlength(pattern);
   bool matched = true;
   size_t signIndex = 0;
   while (matched && i < len) {
      if (pattern[i] == '{') {
         size_t end = pattern.findSub(i, '}', 0);

         String<char, 5> tmp;
         tmp.copy(pattern + i + 1, end - i - 1);

         size_t index = tmp.toInt(10);

         parameters[index - 1] = signatures[signIndex];
         if (argumentLen < index)
            argumentLen = index;

         i = end + 2;
      }
      else {
         size_t end = pattern.findSub(i, '/', getlength(pattern));
         IdentifierString argType;
         argType.copy(pattern + i, end - i);

         if ((*argType).find('{') != NOTFOUND_POS) {
            ref_t argRef = signatures[signIndex];
            // bad luck : if it is a template based argument
            ustr_t signType;
            while (argRef) {
               // try to find the template based signature argument
               signType = scope.module->resolveReference(argRef);
               if (!isTemplateWeakReference(signType)) {
                  ClassInfo info;
                  defineClassInfo(scope, info, argRef, true);
                  argRef = info.header.parentRef;
               }
               else break;
            }

            if (argRef) {
               size_t argLen = argType.length();
               size_t start = 0;
               size_t argIndex = (*argType).find('{');
               while (argIndex < argLen && matched) {
                  if (argType.compare(signType, start, argIndex - start)) {
                     size_t paramEnd = (*argType).findSub(argIndex, '}', 0);

                     String<char, 5> tmp;
                     tmp.copy(*argType + argIndex + 1, paramEnd - argIndex - 1);

                     IdentifierString templateArg;
                     size_t nextArg = readSignatureMember(signType, argIndex - start);
                     templateArg.copy(signType + argIndex - start, nextArg - argIndex + start);
                     decodeClassName(templateArg);

                     signType = signType + nextArg + 1;

                     size_t index = tmp.toInt();
                     ref_t templateArgRef = scope.mapFullReference(*templateArg);
                     if (!parameters[index - 1]) {
                        parameters[index - 1] = templateArgRef;
                     }
                     else if (parameters[index - 1] != templateArgRef) {
                        matched = false;
                        break;
                     }

                     if (argumentLen < index)
                        argumentLen = index;

                     start = paramEnd + 2;
                     argIndex = (*argType).findSub(start, '{', argLen);
                  }
                  else matched = false;
               }

               if (matched && start < argLen) {
                  // validate the rest part
                  matched = argType.compare(signType, start, argIndex - start);
               }
            }
            else matched = false;
         }
         else {
            ref_t argRef = scope.mapFullReference(*argType, true);
            matched = isCompatible(scope, { argRef }, { signatures[signIndex] }, true);
         }

         i = end + 1;
      }

      signIndex++;
   }

   // check if it is assigned
   for (size_t argI = 0; argI < argumentLen; argI++) {
      if (!parameters[argI])
         parameters[argI] = scope.buildins.superReference;
   }

   if (matched) {
      return compiler->generateExtensionTemplate(scope, templateRef, argumentLen, parameters, ns, outerExtensionList);
   }

   return 0;
}

ref_t CompilerLogic :: resolveExtensionTemplateByTemplateArgs(ModuleScopeBase& scope, CompilerBase* compiler, ustr_t pattern, 
   ustr_t ns, size_t argumentLen, ref_t* arguments, ExtensionMap* outerExtensionList)
{
   // matching pattern with the provided signature
   size_t i = pattern.find('.');

   // define an argument length
   size_t argLenPos = pattern.findSub(0, '#', i, NOTFOUND_POS);
   if (i != NOTFOUND_POS) {
      i += 2;

      String<char, 5> tmp;
      tmp.copy(pattern + argLenPos + 1, i - argLenPos - 3);

      if(argumentLen != tmp.toInt())
         return 0;
   }
   else return 0;

   IdentifierString templateName(pattern, i - 2);
   ref_t templateRef = scope.mapFullReference(*templateName, true);

   return compiler->generateExtensionTemplate(scope, templateRef, argumentLen, arguments, ns, outerExtensionList);
}

bool CompilerLogic :: isNumericType(ModuleScopeBase& scope, ref_t& reference)
{
   if (isCompatible(scope, { V_INT8 }, { reference }, false)) {
      reference = V_INT8;

      return true;
   }
   if (isCompatible(scope, { V_UINT8 }, { reference }, false)) {
      reference = V_UINT8;

      return true;
   }
   if (isCompatible(scope, { V_INT16 }, { reference }, false)) {
      reference = V_INT16;

      return true;
   }
   if (isCompatible(scope, { V_INT64 }, { reference }, false) && !isCompatible(scope, { V_PTR64 }, { reference }, false)) {
      // HOTFIX : ignore pointer
      reference = V_INT64;

      return true;
   }
   if (isCompatible(scope, { V_FLOAT64 }, { reference }, false)) {
      reference = V_FLOAT64;

      return true;
   }

   return false;
}

bool CompilerLogic :: isLessAccessible(ModuleScopeBase& scope, Visibility sourceVisibility, ref_t targetRef)
{
   if (!targetRef)
      return false;

   Visibility targetVisibility = scope.retrieveVisibility(targetRef);

   return sourceVisibility > targetVisibility;
}

bool CompilerLogic :: loadMetaData(ModuleScopeBase* moduleScope, ustr_t aliasName, ustr_t nsName)
{
   if (aliasName.compare(PREDEFINED_MAP_KEY)) {
      IdentifierString fullName(nsName, "'", META_PREFIX, PREDEFINED_MAP);

      auto predefinedInfo = moduleScope->getSection(*fullName, mskAttributeMapRef, true);
      if (predefinedInfo.section) {
         readAttributeMap(predefinedInfo.section, moduleScope->predefined);

         return true;
      }
   }
   else if (aliasName.compare(ATTRIBUTES_MAP_KEY)) {
      IdentifierString fullName(nsName, "'", META_PREFIX, ATTRIBUTES_MAP);

      auto attributeInfo = moduleScope->getSection(*fullName, mskAttributeMapRef, true);

      if (attributeInfo.section) {
         readAttributeMap(attributeInfo.section, moduleScope->attributes);

         return true;
      }
   }
   else if (aliasName.compare(OPERATION_MAP_KEY)) {
      IdentifierString fullName(nsName, "'", META_PREFIX, OPERATION_MAP);

      auto operationInfo = moduleScope->getSection(*fullName, mskTypeMapRef, true);

      if (operationInfo.section) {
         readTypeMap(operationInfo.module, operationInfo.section, moduleScope->operations, moduleScope);

         return true;
      }
   }
   else if (aliasName.compare(ALIASES_MAP_KEY)) {
      IdentifierString fullName(nsName, "'", META_PREFIX, ALIASES_MAP);

      auto aliasInfo = moduleScope->getSection(*fullName, mskTypeMapRef, true);

      if (aliasInfo.section) {
         readTypeMap(aliasInfo.module, aliasInfo.section, moduleScope->aliases, moduleScope);

         return true;
      }
   }

   return false;
}

bool CompilerLogic :: clearMetaData(ModuleScopeBase* moduleScope, ustr_t name)
{
   if (name.compare(PREDEFINED_MAP)) {
      moduleScope->predefined.clear();
   }
   else if (name.compare(ATTRIBUTES_MAP)) {
      moduleScope->attributes.clear();
   }
   else if (name.compare(OPERATION_MAP)) {
      moduleScope->operations.clear();
   }
   else if (name.compare(ALIASES_MAP)) {
      moduleScope->aliases.clear();
   }
   else return false;

   return true;
}

ref_t CompilerLogic :: retrievePrimitiveType(ModuleScopeBase& scope, ref_t reference)
{
   if (!reference || isPrimitiveRef(reference))
      return reference;

   ClassInfo info;
   if (!scope.loadClassInfo(info, reference))
      return 0;

   if (isWrapper(info)) {
      auto inner = *info.fields.start();

      if (isPrimitiveRef(inner.typeInfo.typeRef))
         return inner.typeInfo.typeRef;

      return retrievePrimitiveType(scope, inner.typeInfo.typeRef);
   }

   return 0;
}

ref_t CompilerLogic :: loadClassInfo(ClassInfo& info, ModuleInfo& moduleInfo, ModuleBase* target, bool headerOnly, bool fieldsOnly)
{
   if (moduleInfo.unassigned())
      return 0;

   // load argument VMT meta data
   MemoryBase* metaData = moduleInfo.module->mapSection(moduleInfo.reference | mskMetaClassInfoRef, true);
   if (!metaData)
      return 0;

   MemoryReader reader(metaData);
   if (moduleInfo.module != target) {
      ClassInfo copy;
      copy.load(&reader, headerOnly, fieldsOnly);

      importClassInfo(copy, info, moduleInfo.module, target, headerOnly, false/*, false*/);

      // import reference
      ImportHelper::importReference(moduleInfo.module, moduleInfo.reference, target);
   }
   else info.load(&reader, headerOnly, fieldsOnly);

   return moduleInfo.reference;
}

void CompilerLogic :: importClassInfo(ClassInfo& copy, ClassInfo& target, ModuleBase* exporter,
   ModuleBase* importer, bool headerOnly, bool inheritMode)
{
   target.header = copy.header;
   target.size = copy.size;

   if (!headerOnly) {
      // import method references and mark them as inherited if required (inherit mode)
      for (auto it = copy.methods.start(); !it.eof(); ++it) {
         MethodInfo info = *it;

         if (info.outputRef)
            info.outputRef = ImportHelper::importReference(exporter, info.outputRef, importer);

         if (info.multiMethod)
            info.multiMethod = ImportHelper::importMessage(exporter, info.multiMethod, importer);

         if (info.byRefHandler)
            info.byRefHandler = ImportHelper::importMessage(exporter, info.byRefHandler, importer);

         if (inheritMode) {
            info.inherited = true;

            // private methods are not inherited
            if (!MethodInfo::checkVisibility(info, MethodHint::Private))
               target.methods.add(ImportHelper::importMessage(exporter, it.key(), importer), info);
         }
         else target.methods.add(ImportHelper::importMessage(exporter, it.key(), importer), info);
      }

      for (auto it = copy.fields.start(); !it.eof(); ++it) {
         FieldInfo info = *it;

         if (info.typeInfo.typeRef && !isPrimitiveRef(info.typeInfo.typeRef))
            info.typeInfo.typeRef = ImportHelper::importReference(exporter, info.typeInfo.typeRef, importer);

         if (info.typeInfo.elementRef && !isPrimitiveRef(info.typeInfo.elementRef))
            info.typeInfo.elementRef = ImportHelper::importReference(exporter, info.typeInfo.elementRef, importer);

         target.fields.add(it.key(), info);
      }

      for (auto it = copy.attributes.start(); !it.eof(); ++it) {
         ClassAttributeKey key = it.key();
         if (test((unsigned)key.value2, (unsigned)ClassAttribute::ReferenceKeyMask)) {
            key.value1 = ImportHelper::importReference(exporter, key.value1, importer);
         }
         else if (test((unsigned)key.value2, (unsigned)ClassAttribute::MessageKeyMask)) {
            key.value1 = ImportHelper::importMessage(exporter, key.value1, importer);
         }
         ref_t referece = *it;
         if (test((unsigned)key.value2, (unsigned)ClassAttribute::ReferenceMask)) {
            referece = ImportHelper::importReference(exporter, referece, importer);
         }
         else if (test((unsigned)key.value2, (unsigned)ClassAttribute::MessageMask)) {
            referece = ImportHelper::importMessage(exporter, referece, importer);
         }

         target.attributes.add(key, referece);
      }

      for (auto it = copy.statics.start(); !it.eof(); ++it) {
         auto info = *it;
         if (info.typeInfo.typeRef && !isPrimitiveRef(info.typeInfo.typeRef))
            info.typeInfo.typeRef = ImportHelper::importReference(exporter, info.typeInfo.typeRef, importer);

         if (info.typeInfo.elementRef)
            info.typeInfo.elementRef = ImportHelper::importReference(exporter, info.typeInfo.elementRef, importer);

         info.valueRef = ImportHelper::importReferenceWithMask(exporter, info.valueRef, importer);

         target.statics.add(it.key(), info);
      }
   }

   // import class class reference
   if (target.header.classRef != 0)
      target.header.classRef = ImportHelper::importReference(exporter, target.header.classRef, importer);

   // import parent reference
   if (target.header.parentRef)
      target.header.parentRef = ImportHelper::importReference(exporter, target.header.parentRef, importer);
}

pos_t CompilerLogic :: definePadding(ModuleScopeBase& scope, pos_t offset, pos_t size)
{
   switch (size) {
      case 1:
         return 0;
      case 2:
      case 4:
      case 8:
         return align(offset, size) - offset;
      default:
         return align(offset, scope.ptrSize) - offset;
   }
}

bool CompilerLogic :: validateDispatcherType(ClassInfo& classInfo)
{
   bool isProxy = classInfo.fields.count() == 1 && test(classInfo.header.flags, elWithCustomDispatcher | elNestedClass | elSealed)
         && !testany(classInfo.header.flags, elWithGenerics | elWithVariadics | elStructure);

   if (isProxy && (classInfo.header.flags & elDebugMask) == 0) {
      classInfo.header.flags |= elProxy;

      return true;
   }

   return false;
}
