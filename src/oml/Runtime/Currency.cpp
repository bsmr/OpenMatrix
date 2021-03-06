/**
* @file Currency.h
* @date August 2013
* Copyright (C) 2013-2018 Altair Engineering, Inc.  
* This file is part of the OpenMatrix Language (�OpenMatrix�) software.
* Open Source License Information:
* OpenMatrix is free software. You can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* OpenMatrix is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.
* You should have received a copy of the GNU Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
* 
* Commercial License Information: 
* For a copy of the commercial license terms and conditions, contact the Altair Legal Department at Legal@altair.com and in the subject line, use the following wording: Request for Commercial License Terms for OpenMatrix.
* Altair�s dual-license business model allows companies, individuals, and organizations to create proprietary derivative works of OpenMatrix and distribute them - whether embedded or bundled with other software - under a commercial license agreement.
* Use of Altair�s trademarks and logos is subject to Altair's trademark licensing policies.  To request a copy, email Legal@altair.com and in the subject line, enter: Request copy of trademark and logo usage policy.
*/

#include "Currency.h"

#include "FunctionInfo.h"
#include "CellDisplay.h"
#include "CurrencyDisplay.h"
#include "MatrixDisplay.h"
#include "MatrixNDisplay.h"
#include "OutputFormat.h"
#include "StringDisplay.h"
#include "StructData.h"
#include "StructDisplay.h"
#include "GeneralFuncs.h"

#include "hwMatrixN.h"

#include <cassert>
#include <sstream>
#include <iomanip>

#ifndef OS_WIN
#define sprintf_s sprintf
#endif

StringManager Currency::vm;
StringManager Currency::pm;
bool Currency::_experimental = false;

Currency::Currency(double val): type(TYPE_SCALAR),  mask(MASK_DOUBLE), out_name(NULL)
    , _display (0)
    , _outputType (OUTPUT_TYPE_DEFAULT)
{
	data.value = val;
}

Currency::Currency(int val): type(TYPE_SCALAR), mask(MASK_DOUBLE), out_name(NULL)
    , _display (0)
    , _outputType (OUTPUT_TYPE_DEFAULT)
{
	data.value = val;
}

Currency::Currency(size_t val): type(TYPE_SCALAR), mask(MASK_DOUBLE), out_name(NULL)
    , _display (0)
    , _outputType (OUTPUT_TYPE_DEFAULT)
{
	data.value = (double)val;
}

Currency::Currency(double val, int in_type): type(CurrencyType(in_type)), mask(MASK_DOUBLE), out_name(NULL)
    , _display (0)
    , _outputType (OUTPUT_TYPE_DEFAULT)
{
	data.value = val;
}

Currency::Currency(double val, std::string info): type(TYPE_ERROR), mask(MASK_NONE), out_name(NULL)
    , _display (0)
    , _outputType (OUTPUT_TYPE_DEFAULT)
{
	message = new std::string;
	*message = info;
	data.value = val;
}

Currency::Currency(bool logical_val) : type(TYPE_SCALAR), mask(MASK_LOGICAL), out_name(NULL)
    , _display (0)
    , _outputType (OUTPUT_TYPE_DEFAULT)
{
	data.value = logical_val ? 1 : 0;
}

Currency::Currency(const char* in_str): type(TYPE_MATRIX), mask(MASK_STRING), out_name(NULL)
    , _display (0)
    , _outputType (OUTPUT_TYPE_DEFAULT)
{
	std::string str = in_str;
    data.mtx = ExprTreeEvaluator::allocateMatrix(str.length() ? 1 : 0, (int)str.length(), hwMatrix::REAL);
	for (unsigned int i = 0; i < str.length(); i++) 
		(*data.mtx)(i) = (unsigned char) str[i];
}

Currency::Currency(const std::string& str): type(TYPE_MATRIX), mask(MASK_STRING), out_name(NULL)
    , _display (0)
    , _outputType (OUTPUT_TYPE_DEFAULT)
{
    data.mtx = ExprTreeEvaluator::allocateMatrix(str.length() ? 1 : 0, (int)str.length(), hwMatrix::REAL);
	for (unsigned int i = 0; i < str.length(); i++) 
		(*data.mtx)(i) = (unsigned char) str[i];
}

Currency::Currency(const std::vector<double>& in_data): type(TYPE_MATRIX), mask(MASK_DOUBLE), out_name(NULL)
    , _display (0)
    , _outputType (OUTPUT_TYPE_DEFAULT)
{ 
	data.mtx = ExprTreeEvaluator::allocateMatrix(1, (int)in_data.size(), hwMatrix::REAL);
	for (unsigned int i = 0; i < in_data.size(); i++) 
		(*data.mtx)(i) = in_data[i];
}

Currency::Currency(hwMatrix* in_data): type (TYPE_MATRIX), mask(MASK_DOUBLE), out_name(NULL)
    , _display (0)
    , _outputType (OUTPUT_TYPE_DEFAULT)
{
	data.mtx = in_data;
}

Currency::Currency(hwMatrixN* in_data): type (TYPE_ND_MATRIX), mask(MASK_DOUBLE), out_name(NULL)
    , _display (0)
    , _outputType (OUTPUT_TYPE_DEFAULT)
{
	data.mtxn = in_data;

	hwMatrix* temp = ConvertNDto2D(data.mtxn);
	
	if (temp)
	{
		DeleteMatrixN(data.mtxn);
		data.mtx = temp;
		type = TYPE_MATRIX;
	}
}

Currency::Currency(const hwComplex& cplx): type (TYPE_COMPLEX), mask(MASK_DOUBLE), out_name(NULL)
    , _display (0)
    , _outputType (OUTPUT_TYPE_DEFAULT)
{
	data.complex = new hwComplex(cplx);
}

Currency::Currency(): type(TYPE_MATRIX), mask(MASK_DOUBLE), out_name(NULL)
    , _display (0)
    , _outputType (OUTPUT_TYPE_DEFAULT)
{
	data.mtx = NULL; 
}

Currency::Currency(HML_CELLARRAY* cell_array): type(TYPE_CELLARRAY), mask(MASK_NONE), out_name(NULL)
    , _display (0)
    , _outputType (OUTPUT_TYPE_DEFAULT)
{
	data.cells = cell_array;
}

Currency::Currency(FunctionInfo* fi): type(TYPE_FUNCHANDLE), mask(MASK_NONE), out_name(NULL)
    , _display (0)
    , _outputType (OUTPUT_TYPE_DEFAULT)
{
	data.func = fi;
	data.func->IsNested(false);
}

Currency::Currency(StructData* in_data): type(TYPE_STRUCT), mask(MASK_NONE), out_name(NULL)
    , _display (0)
    , _outputType (OUTPUT_TYPE_DEFAULT)
{
	data.sd = in_data;
}

Currency::Currency(OutputFormat* fmt) : type(TYPE_FORMAT), mask(MASK_NONE), out_name(NULL)
    , _display (0)
    , _outputType (OUTPUT_TYPE_DEFAULT)
{
	data.format = fmt;
}

Currency::Currency(Currency* ptr) : type(TYPE_POINTER), out_name(NULL)
    , _display (0)
    , _outputType (OUTPUT_TYPE_DEFAULT)
{
	data.cur_ptr = ptr;
}
//------------------------------------------------------------------------------
//! Constructor for swig bound objects
//! \param[in] obj  Pointer to bound object
//! \param[in] name Class name associated with the bound object
//------------------------------------------------------------------------------
Currency::Currency(void* obj, const std::string& name) 
    : type         (TYPE_BOUNDOBJECT)
    , out_name     (NULL)
    , _display     (0)
    , _outputType  (OUTPUT_TYPE_DEFAULT)
    , classname    (NULL)
{
    assert(!name.empty());

    data.boundobj = obj;
	classname     = const_cast<std::string*>(vm.GetStringPointer(name));
}
//------------------------------------------------------------------------------
//! Copies given currency
//! \param[in] cur Given currency
//------------------------------------------------------------------------------
void Currency::Copy(const Currency& cur)
{
	hwMatrix*      old_matrix   = NULL;
	hwMatrixN*     old_matrix_n = NULL;
	HML_CELLARRAY* old_cells    = NULL;
	StructData*    old_sd       = NULL;
	hwComplex*     old_complex  = NULL;
	bool           was_scalar   = false;
	FunctionInfo*  old_fi       = NULL;

	if (type == TYPE_SCALAR)
		was_scalar = true;
	else if (type == TYPE_MATRIX)
		old_matrix = data.mtx;
	else if (type == TYPE_ND_MATRIX)
			old_matrix_n = data.mtxn;
	else if (type == TYPE_CELLARRAY)
		old_cells = data.cells;
	else if ((type == TYPE_STRUCT) || (type == TYPE_OBJECT))
		old_sd = data.sd;
	else if (type == TYPE_COMPLEX)
		old_complex = data.complex;
	else if (type == TYPE_FUNCHANDLE)
		old_fi = data.func;

	type             = cur.type;
	mask             = cur.mask;
	out_name         = cur.out_name;
    _display         = NULL; // Do not copy display, it will be set later
    _outputType      = cur._outputType;

	if (type == TYPE_SCALAR)
	{
		data.value   = cur.data.value;
	}
	else if (type == TYPE_MATRIX)
	{
		data.mtx = cur.data.mtx;

		if (data.mtx)
			data.mtx->IncrRefCount();
	}
	else if (type == TYPE_ND_MATRIX)
	{
		data.mtxn = cur.data.mtxn;

		if (data.mtxn)
			data.mtxn->IncrRefCount();
	}
	else if (type == TYPE_COMPLEX)
	{
		data.complex = new hwComplex(cur.Real(), cur.Imag());
	}
	else if (type == TYPE_CELLARRAY)
	{
		data.cells = cur.data.cells;

		if (data.cells)
			data.cells->IncrRefCount();
	}
	else if (type == TYPE_STRUCT)
	{
		data.sd = cur.data.sd;

		if (data.sd)
			data.sd->IncrRefCount();
	}
	else if (type == TYPE_OBJECT)
	{
		data.sd = cur.data.sd;

		if (data.sd)
			data.sd->IncrRefCount();

		classname = cur.classname;
	}
	else if (type == TYPE_FORMAT)
	{
        data.format = new OutputFormat(*cur.data.format);
	}
	else if (type == TYPE_POINTER)
	{
		data.cur_ptr = cur.data.cur_ptr;
	}
	else if (type == TYPE_FUNCHANDLE)
	{
		data.func	= new FunctionInfo(*cur.data.func);
	}
	else if (type == TYPE_ERROR)
	{
		message  = new std::string;
		*message = cur.Message();
	}
    else if (type == TYPE_BOUNDOBJECT)
    {
        data.boundobj = cur.data.boundobj; //\todo: Add ref counting
        classname     = cur.classname;
    }

	if (was_scalar)
		;
	else if (old_matrix && (type != TYPE_POINTER))
		DeleteMatrix(old_matrix);
	else if (old_matrix_n && (type != TYPE_POINTER))
		DeleteMatrixN(old_matrix_n);
	else if (old_cells && (type != TYPE_POINTER))
		DeleteCells(old_cells);
	else if (old_sd && (type != TYPE_POINTER))
		DeleteStruct(old_sd);
	else if (old_complex && (type != TYPE_POINTER))
		delete old_complex;
	else if (old_fi && (type != TYPE_POINTER))
		delete old_fi;
}

void Currency::DeleteMatrix(hwMatrix* matrix)
{
	if (matrix)
	{
		if (!matrix->IsMatrixShared())
		{
			delete matrix;

			if (matrix == data.mtx)
				data.mtx = NULL;
		}
		else
		{
			matrix->DecrRefCount();
		}
	}
}

void Currency::DeleteMatrixN(hwMatrixN* matrix)
{
	if (matrix)
	{
		if (!matrix->IsMatrixShared())
		{
			delete matrix;

			if (matrix == data.mtxn)
				data.mtxn = NULL;
		}
		else
		{
			matrix->DecrRefCount();
		}
	}
}

void Currency::DeleteCells(HML_CELLARRAY* cells)
{
	if (cells)
	{
		if (!cells->IsMatrixShared())
		{
			delete cells;

			if (cells == data.cells)
				data.cells = NULL;
		}
		else
		{
			cells->DecrRefCount();
		}
	}
}

void Currency::DeleteStruct(StructData* sd)
{
	if (sd)
	{
		if (sd->GetRefCount() == 1)
		{
			delete sd;

			if (sd == data.sd)
				data.sd = NULL;
		}
		else
		{
			sd->DecrRefCount();
		}
	}
}

Currency::Currency(const Currency& cur)
{
	type = TYPE_SCALAR;
	Copy(cur);
}

Currency& Currency::operator=(const Currency& in)
{
	Copy(in);
	return *this;
}

Currency::~Currency()
{	
	if (type == TYPE_MATRIX)
		DeleteMatrix(data.mtx);
	else if (type == TYPE_SCALAR)
		;
	else if (type == TYPE_ND_MATRIX)
		DeleteMatrixN(data.mtxn);
	else if (type == TYPE_COMPLEX)
        delete data.complex;
	else if ((type == TYPE_STRUCT) || (type == TYPE_OBJECT))
		DeleteStruct(data.sd);
    else if (type == TYPE_FORMAT)
        delete data.format;
	else if (type == TYPE_CELLARRAY)
		DeleteCells(data.cells);
	else if (type == TYPE_ERROR)
		delete message;
	else if (type == TYPE_FUNCHANDLE)
		delete data.func;
}

void Currency::ReplaceComplex(hwComplex new_value)
{
	if (!data.complex)
		data.complex = new hwComplex;

	data.complex->Real(new_value.Real());
	data.complex->Imag(new_value.Imag());

	type = TYPE_COMPLEX;
}
//------------------------------------------------------------------------------
//! Returns a string description of the currency type
//------------------------------------------------------------------------------
std::string Currency::GetTypeString() const
{
    std::string output;
	if (IsScalar())
	{
		if (IsLogical())
			output = "logical";
		else
			output = "number";
	}
	else if (IsString())
	{
		output = "string";
	}
	else if (IsComplex())
	{
		output = "complex";
	}
	else if (IsMatrix())
	{
		int rows = 0;
		int cols = 0;
		const hwMatrix* mtx = Matrix();

		if (mtx)
		{
			rows = mtx->M();
			cols = mtx->N();
		}

		char buffer[1024];
		sprintf_s(buffer, "matrix [%d x %d]", rows, cols);
		output = buffer;
	}
	else if (IsFunctionHandle())
	{
		output = "function handle";
	}
	else if (IsCellArray())
	{
		HML_CELLARRAY* mtx = CellArray();
		char buffer[1024];
		sprintf_s(buffer, "cell array [%d x %d]", mtx->M(), mtx->N());
		output = buffer;
	}
	else if (IsStruct())
	{
		StructData* mtx = Struct();
		char buffer[1024];
		sprintf_s(buffer, "struct [%d x %d]", mtx->M(), mtx->N());
		output = buffer;
	}
    else if (IsNDMatrix()) 
    {
        std::ostringstream strstream;
        strstream << "matrix [";
        const hwMatrixN* mtxN = MatrixN();
        if( mtxN ) 
        {
            const std::vector<int>& dims = mtxN->Dimensions();
            size_t size = dims.size();
            for(int i = 0; i < size; ++i) 
            {
                strstream << dims[i];
                if( i < size-1 ) strstream << " x ";
            }
        }
        strstream << "]" << std::ends;
        output = strstream.str();
    }
    else if (IsBoundObject())
        return GetClassname();

	return output;
}
//------------------------------------------------------------------------------
//! Returns true if currency is type
//------------------------------------------------------------------------------
bool Currency::IsString()    const
{
	return (mask == MASK_STRING && data.mtx);
}

bool Currency::IsMultilineString() const
{
	bool ret_val = IsString();

	if (ret_val)
	{
		hwMatrix* mtx = data.mtx;

		if (mtx->M() == 1)
			ret_val = false;
	}
	return ret_val;
}
//------------------------------------------------------------------------------
//! Returns the string value for the currency
//------------------------------------------------------------------------------
std::string Currency::StringVal() const
{
    if (!data.mtx) return "";

	std::string st;

	int rows = data.mtx->M();
	int cols = data.mtx->N();

	if (rows > 1) 
		st += "\n";  // Add newline if there are multiple rows

	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			if ((*data.mtx)(i, j) != 0x00)
            {
				st += static_cast<unsigned char>((*data.mtx)(i,j));
            }
		}

		if (i+1 < rows)
			st += "\n";
	}

 	return st;
}

bool Currency::IsVector() const
{
	bool ret_val = false;

	// not sure if this should return true or not for scalar.  Technically,
	// a scalar is a vector of size 1
	if (type == TYPE_MATRIX && data.mtx && mask != MASK_STRING)
	{
		if ((data.mtx->M() == 1) || (data.mtx->N() == 1))
			ret_val = true;
	}

	return ret_val;
}

bool Currency::IsRealVector() const
{
	bool ret_val = false;

	if (type == TYPE_MATRIX && data.mtx && mask != MASK_STRING)
	{
		if ((data.mtx->M() == 1) || (data.mtx->N() == 1))
			ret_val = true;

		if (!data.mtx->IsReal())
			ret_val = false;
	}

	return ret_val;
}

std::vector<double> Currency::Vector() const
{ 
	std::vector<double> v;

	if (data.mtx->M() == 1)
	{
		for (int i=0; i < data.mtx->N(); i++)
			v.push_back ((*data.mtx)(0,i));
	}
	else if (data.mtx->N() == 1)
	{
		for (int i=0; i < data.mtx->M(); i++)
			v.push_back ((*data.mtx)(i,0));
	}
	return v; 
}

void Currency::MakeStruct()
{
	if (type != TYPE_STRUCT)
	{
		if (type == TYPE_CELLARRAY)
			DeleteCells(data.cells);
		else if (type == TYPE_MATRIX)
			DeleteMatrix(data.mtx);

		type = TYPE_STRUCT;
		mask = MASK_NONE;
		data.sd = new StructData;
	}
}

void Currency::MakeCell()
{
	if (type != TYPE_CELLARRAY)
	{
		type = TYPE_CELLARRAY;
		mask = MASK_NONE;
		data.cells = EvaluatorInterface::allocateCellArray(1, 1);
	}
}

bool Currency::IsScalar() const
{
	if (type == TYPE_SCALAR)
		return true;

	if ((type == TYPE_COMPLEX) && (data.complex->Imag() == 0.0))
		return true;

	if (type == TYPE_MATRIX && mask != MASK_STRING)
	{
		if (data.mtx && (data.mtx->Size() == 1) && (data.mtx->IsReal()))
			return true;
	}

	return false;
}

double Currency::Scalar() const
{
	if (type == TYPE_SCALAR)
		return data.value;

	if ((type == TYPE_COMPLEX) && (data.complex->Imag() == 0.0))
		return data.complex->Real();

	if ((type == TYPE_MATRIX) && data.mtx && (data.mtx->Size() == 1) && (data.mtx->IsReal()))
		return (*data.mtx)(0);

	return 0.0;
}

bool Currency::IsComplex() const
{
	if (type == TYPE_COMPLEX)
		return true;

	if (type == TYPE_MATRIX && mask != MASK_STRING)
	{
		if (data.mtx && (data.mtx->Size() == 1) && (!data.mtx->IsReal()))
			return true;
	}

	return false;
}

hwComplex Currency::Complex() const
{
	if (type == TYPE_COMPLEX)
		return *data.complex;

	if ((type == TYPE_MATRIX) && data.mtx && (data.mtx->Size() == 1) && (!data.mtx->IsReal()))
		return data.mtx->z(0);

	return hwComplex(0.0, 0.0);
}

bool Currency::IsInteger() const
{
	if (IsScalar())
	{
		double temp = Scalar();

    	if (abs(temp - (int(temp+1.0e-15)) < 1.0e-15))
			return true;
	}

	return false;
}

bool Currency::IsPositiveInteger() const
{
	if (IsScalar())
	{
		double temp = Scalar();

		if (temp > 0.0)
		{
			if (abs(temp - (int(temp+1.0e-15)) < 1.0e-15))
				return true;
		}
	}

	return false;
}

bool Currency::IsPositiveVector() const
{
	if (IsLogical())
		return false;

	if (IsMatrix() && !IsString())
	{
		const hwMatrix* mtx = Matrix();

		if (!mtx->IsReal())
			return false;

		if (mtx->IsEmpty())
			return false;

		if ((mtx->M() != 1) && (mtx->N() != 1))
			return false;

        for (int k=0; k<mtx->N(); k++)
        {
            for (int j=0; j<mtx->M(); j++)
            {
				double val = (*mtx)(j,k);

				if (val <= 0.0)
					return false;

				// making sure it's an integer (or close enough)
				if (abs(val - (int(val+1.0e-15)) > 1.0e-15))
					return false;				
			}
		}

		return true;
	}

	return false;
}

bool Currency::IsEmpty() const
{
	if (IsMatrix())
	{
		if (data.mtx)
			return data.mtx->IsEmpty();
		else
			return true;
	}

	return false;
}

bool  Currency::IsMatrix() const 
{ 
	if (mask == MASK_STRING)
		return false;
	else
		return type == TYPE_MATRIX; 
}

bool  Currency::IsNDMatrix() const 
{ 
	// ND matrices can't be strings so don't bother checking
	return type == TYPE_ND_MATRIX; 
}

bool Currency::IsCharacter() const 
{
	if (IsString())
	{
		if (data.mtx->Size() == 1)
			return true;
	}

	return false;
}

const hwMatrix* Currency::Matrix() const
{
	if (type == TYPE_MATRIX && !data.mtx)
		data.mtx = new hwMatrix();

	return data.mtx;
}

const hwMatrixN* Currency::MatrixN() const
{
	return data.mtxn;
}

hwMatrix* Currency::GetWritableMatrix() 
{
	if (!data.mtx)
		data.mtx = ExprTreeEvaluator::allocateMatrix();

	return data.mtx;
}

hwMatrixN* Currency::GetWritableMatrixN() 
{
	if (!data.mtxn)
		data.mtxn = ExprTreeEvaluator::allocateMatrixN();

	return data.mtxn;
}

void Currency::ReplaceMatrix(hwMatrix* new_mtx)
{
	if (type == TYPE_MATRIX)
	{
		if (new_mtx != data.mtx)
		{
			DeleteMatrix(data.mtx);
			data.mtx = new_mtx;
		}
	}
	else if (type == TYPE_SCALAR)
	{
		type = TYPE_MATRIX;
		data.mtx = new_mtx;
	}
	else if (type == TYPE_COMPLEX)
	{
		type = TYPE_MATRIX;
		data.mtx = new_mtx;
	}
}

void Currency::ReplaceCellArray(HML_CELLARRAY* new_cells)
{
	if (type == TYPE_CELLARRAY)
	{
		if (new_cells != data.cells)
		{
			DeleteCells(data.cells);
			data.cells = new_cells;
		}
	}
}

void Currency::ReplaceStruct(StructData* new_sd)
{
	if ((type == TYPE_STRUCT) || (type == TYPE_OBJECT))
	{
		if (new_sd != data.sd)
		{
			DeleteStruct(data.sd);
			data.sd = new_sd;
		}
	}
}

bool Currency::IsLogical() const
{
	if (mask == MASK_LOGICAL)
		return true;
	else
		return false;
}

bool Currency::IsSyntaxError() const
{
	if (IsError() && data.value == -999.0)
		return true;

	return false;
}

bool Currency::IsCellList() const
{
    if (IsCellArray() && (mask == MASK_CELL_LIST))
		return true;

	return false;
}

const hwMatrix* Currency::ConvertToMatrix() const
{
	if (type == TYPE_SCALAR)
	{
		double old_val = data.value;
		data.mtx = ExprTreeEvaluator::allocateMatrix(1,1, hwMatrix::REAL);
		(*data.mtx)(0) = old_val;
	}
	else if (type == TYPE_COMPLEX)
	{
		hwComplex* old_complex = data.complex;
		data.mtx = ExprTreeEvaluator::allocateMatrix(1,1, hwMatrix::COMPLEX);
		data.mtx->z(0) = *old_complex;
	}
	else if (type == TYPE_MATRIX)
	{
	}
	else if (IsCellList())
	{
		HML_CELLARRAY* cells   = CellArray();
		bool           is_real = true;

		int size = cells->Size();

		int size_needed = 0;

		for (int j=0; j<size; j++)
		{
			Currency temp = (*cells)(j);

			if (temp.IsScalar())
			{
				size_needed++;
			}
			else if (temp.IsComplex())
			{
				size_needed++;
			}
			else if (temp.IsString())
			{
				const hwMatrix* string = temp.Matrix();
				size_needed += string->N();
			}
			else
			{
				return NULL;
			}
		}

		hwMatrix* mtx = ExprTreeEvaluator::allocateMatrix(1, size_needed, hwMatrix::REAL);
		int count = 0;

		for (int j=0; j<size; j++)
		{
			Currency temp = (*cells)(j);

			if (temp.IsScalar())
			{
				if (mtx->IsReal())
					(*mtx)(count) = temp.Scalar();
				else
					mtx->z(count) = temp.Scalar();
				count++;
			}
			else if (temp.IsComplex())
			{
				mtx->MakeComplex();
				mtx->z(count) = temp.Complex();
				count++;
			}
			else if (temp.IsString())
			{
				const hwMatrix* string = temp.Matrix();
				
				for (int k=0; k<string->N(); k++)
				{
					(*mtx)(count) = (*string)(k);
					count++;
				}
			}
		}

		if (!cells->IsMatrixShared())
			delete cells;
		else
			cells->DecrRefCount();

		data.mtx = mtx;
	}
	else
	{
		data.mtx = NULL;
	}

	type = TYPE_MATRIX;
	return data.mtx;
}

HML_CELLARRAY* Currency::ConvertToCellArray()
{
	bool ret_val = true;

	if (type == TYPE_SCALAR)
	{
		double old_val = data.value;
		data.cells = ExprTreeEvaluator::allocateCellArray(1,1);
		(*data.cells)(0) = old_val;
	}
	else if (type == TYPE_COMPLEX)
	{
		hwComplex* old_complex = data.complex;
		data.cells = ExprTreeEvaluator::allocateCellArray(1,1);
		(*data.cells)(0) = *old_complex;
	}
	else if (type == TYPE_MATRIX)
	{
		hwMatrix* old_mtx = data.mtx;
		data.cells = ExprTreeEvaluator::allocateCellArray(1,1);
		(*data.cells)(0) = old_mtx;

		if (mask == MASK_STRING)
		{
			(*data.cells)(0).SetMask(MASK_STRING);
			mask = MASK_NONE;
		}
	}
	else if (type == TYPE_CELLARRAY)
	{
	}
	else
	{
		data.cells = NULL;
	}

	type = TYPE_CELLARRAY;

	return data.cells;
}

void Currency::ConvertToStruct()
{
	if (IsStruct())
	{
		return;
	}
	else if (IsCellArray())
	{
		HML_CELLARRAY* cells = CellArray();

		if (cells->Size() == 0)
		{
			delete cells;
			data.sd = new StructData;
			type = TYPE_STRUCT;
		}
	}
	else if (IsMatrix())
	{
		hwMatrix* mtx = data.mtx;

	    if (mtx && mtx->Size() == 0)
			delete mtx;

		data.sd = new StructData;
		type = TYPE_STRUCT;
	}
}

void Currency::SetOutputName(const std::string& name) const
{
	out_name = vm.GetStringPointer(name);
}

void Currency::SetOutputName(const std::string* name) const
{
	out_name = name;
}

void Currency::ClearOutputName()
{
   static const std::string* empty_str = NULL;
   
   if (!empty_str)
	   empty_str = vm.GetStringPointer("");

   out_name = empty_str;
}

std::string Currency::GetOutputName() const
{
	if (out_name)
		return *out_name;
	else
		return "";
}

hwMatrix* ConvertNDto2D(const hwMatrixN* mtxn)
{
	// Dimensions will omit any trailing dimensions of size 1
	std::vector<int> dims = mtxn->Dimensions();
	size_t num_dims = dims.size();
	
	if (num_dims > 2)
		return NULL;

	int dim_1 = dims[0];
	int dim_2 = 1;

	if (num_dims == 2)
		dim_2 = dims[1];

	hwMatrix* result = NULL;

	if (mtxn->IsReal())
	{
		result = EvaluatorInterface::allocateMatrix(dim_1, dim_2, hwMatrix::REAL);

		for (int j=0; j<dim_1*dim_2; j++)
			(*result)(j) = (*mtxn)(j);
	}
	else
	{
		result = EvaluatorInterface::allocateMatrix(dim_1, dim_2, hwMatrix::COMPLEX);

		for (int j=0; j<dim_1*dim_2; j++)
			result->z(j) = mtxn->z(j);
	}

	return result;
}
//------------------------------------------------------------------------------
// Gets output for printing
//------------------------------------------------------------------------------
std::string Currency::GetOutputString(const OutputFormat* fmt) const
{
	std::ostringstream os;

	std::string output_name (GetOutputName());

    if (!output_name.empty() && !IsDispOutput() && !IsError() && !IsPrintfOutput())
			os << output_name << " = ";

	if (IsScalar())
	{
        return CurrencyDisplay::ScalarToString(fmt, Scalar(), os);
	}
	else if (IsComplex())
	{
        return CurrencyDisplay::ComplexToString(fmt, Complex(), os);
	}
	else if (IsString())
	{
        std::string val (StringVal());
        if (CurrencyDisplay::CanPaginate(val))
        {
            return (GetDisplay()->GetOutput(fmt, os));
        }
        else
        {
		    os << val;
        }
	}
	else if (IsMatrix() || IsCellArray() || IsStruct() || IsNDMatrix() ||
             IsObject())
	{
        return (GetDisplay()->GetOutput(fmt, os));
	}
    else if (IsFunctionHandle())
	{
		// This should never happen, but there's some weird UI cases that actually do trigger it
		if (data.func)
			os << "@" << data.func->FunctionName();
	}
	else if (IsError())
	{
		os << *message;
	}
    else if (IsBoundObject())
    {
        os << GetClassname();
    }
    std::string output (os.str());
	return output;
}
const std::string* StringManager::GetStringPointer(const std::string& var)
{
	StringStorage::iterator iter;

	iter = _strings.find(&var);

	if (iter == _strings.end())
	{
		std::string* new_str = new std::string(var);
		_strings.insert(new_str);
		return new_str;
	}
	else
	{
		return (*iter);
	}
}
//------------------------------------------------------------------------------
//! Creates, if needed and returns display
//------------------------------------------------------------------------------
CurrencyDisplay* Currency::GetDisplay() const
{
    if (_display) return _display;

    if (IsCellArray())
        _display = new CellDisplay(*this);
    
    else if (IsMatrix())
        _display = new MatrixDisplay(*this);

    else if (IsStruct() || IsObject())
        _display = new StructDisplay(*this);

    else if (IsNDMatrix())
        _display = new MatrixNDisplay(*this);

    else if (IsString())
    {
        std::string val (StringVal());
        if (CurrencyDisplay::CanPaginate(val))
        {
            _display = new StringDisplay(*this);
        }
    }

    return _display;
}
//------------------------------------------------------------------------------
//! Deletes display
//------------------------------------------------------------------------------
void Currency::DeleteDisplay()
{
    delete _display;
    _display = 0;
}

void Currency::SetClass(const std::string& class_name)
{
	type = TYPE_OBJECT;
	classname = (std::string*)vm.GetStringPointer(class_name);
}
//------------------------------------------------------------------------------
//! Utility to get just the values, without any header(s), if applicable
//------------------------------------------------------------------------------
std::string Currency::GetValues(const OutputFormat* fmt) const
{    
    if (IsScalar() || IsComplex())
    {
        return GetOutputString(fmt);
    }

    bool stringpaginate = (IsString() && CurrencyDisplay::CanPaginate(StringVal()));

	if (IsMatrix() || IsCellArray() || IsStruct() || IsNDMatrix() || 
        IsObject() || stringpaginate)
    {
        std::string out = GetDisplay()->GetValues(fmt);
        const_cast<Currency*>(this)->DeleteDisplay();
        return out;
    }

    return GetOutputString(fmt);
} 
//------------------------------------------------------------------------------
// Utility to get just the values without any headers or currency name
//------------------------------------------------------------------------------
std::string Currency::GetValuesForDisplay(const OutputFormat* fmt) const
{    
    OUTPUT_TYPE origtype = _outputType;

    const_cast<Currency*>(this)->_outputType = OUTPUT_TYPE_DISP;
    std::string out = GetOutputString(fmt);
    const_cast<Currency*>(this)->_outputType = origtype;

    return out;
} 

StringManager::~StringManager()
{
	StringStorage::iterator iter;

	for (iter = _strings.begin(); iter != _strings.end(); iter++)
		delete *iter;
}