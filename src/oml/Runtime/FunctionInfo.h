/**
* @file FunctionInfo.h
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

#ifndef __FunctionInfo_h
#define __FunctionInfo_h

#include <string>
#include <vector>
#include "Evaluator.h"

class MemoryScope;

class FunctionStatements
{
public:
	FunctionStatements(OMLTree* stmts);
	~FunctionStatements();

	void IncrRefCnt() { _refcnt++; }
	void DecrRefCnt() { _refcnt--; }
	int  GetRefCnt() const { return _refcnt; }

	OMLTree* Statements() const { return _statements; }

private:
	OMLTree* _statements;
	int      _refcnt;              
};

class HML2DLL_DECLS FunctionInfo 
{
public:
	FunctionInfo(std::string, std::vector<const std::string*>, std::vector<const std::string*>, std::map<const std::string*, Currency>, OMLTree*, std::string, std::string);
	FunctionInfo(std::string, std::vector<const std::string*>, std::vector<const std::string*>, OMLTree*, std::string);
	FunctionInfo(std::string, FUNCPTR);
	FunctionInfo();
	FunctionInfo(const FunctionInfo&);
	~FunctionInfo();

	void SetAnonymous(MemoryScope* dummy) { _anon_scope = new MemoryScope(*dummy, NULL); }
	bool IsAnonymous() const { return _anon_scope != 0; }
	bool IsNested() const { return _is_nested; }
	void IsNested(bool nest) { _is_nested = nest; }
	bool IsBuiltIn() const { return _builtin != 0; }
	bool IsLocalFunction(std::string script_name) const;
	bool IsReturnValue(const std::string* varname) const;
	bool IsInputParameter(const std::string* varname) const;
	bool IsReferenced(const std::string* varname) const;
	int  Nargin()  const;
	int  Nargout() const;

	std::string                     FunctionName() const { return *_function_name; }
	const std::string*              FunctionNamePtr() const { return _function_name; }
	std::string                     FileName() const { return *_file_name; }
	const std::string*              FileNamePtr() const { return _file_name; }
	std::string                     HelpString() const { return _help_string; }
	std::vector<const std::string*> Parameters() const { return _parameters; }
	std::vector<const std::string*> ReturnValues() const { return _return_values; }
	OMLTree*                        Statements() const;
	FUNCPTR                         Builtin() const { return _builtin; }
	MemoryScope*                    AnonScope() const { return _anon_scope; }

	void ClearAnonymousVariable(const std::string* var);

	std::string GetAST() const;

	void SetAsConstructor() { _is_constructor = true; }
	bool IsConstructor() const { return _is_constructor; }

	std::string        RedirectedFunction() const;
	int                NumRedirectedInputs() const;
	OMLTree*           RedirectedInput(int index) const;

	int                MinimumInputs() const;

	MemoryScope* Persistent() const { return _persistent_scope; }

	bool     HasDefaultValue(const std::string*) const;
	Currency GetDefaultValue(const std::string*) const;

	std::map<const std::string*, FunctionInfo*>* local_functions;

private:

	const std::string*              _function_name;
	const std::string*              _file_name;
	std::string					    _help_string;

	std::vector<const std::string*>        _return_values;
	std::vector<const std::string*>        _parameters;
	std::map<const std::string*, Currency> _default_values;

	FunctionStatements*             _stmts;
	FUNCPTR                         _builtin;
	MemoryScope*                    _anon_scope;
	MemoryScope*                    _persistent_scope;
	bool                            _is_nested;
	bool                            _is_constructor;
}; 

#endif