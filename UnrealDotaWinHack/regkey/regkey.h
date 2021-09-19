// regkey.h - Registry Key wrapper
// Copyright (c) 2005 Keith A. Lewis mailto:kal@kalx.net

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <tchar.h>
#include <windows.h>
#include <algorithm>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include <cassert>

#define ensure assert

// might need to ifdef!!!
typedef unsigned __int64 QWORD;
typedef std::basic_string<TCHAR> Tstring;

using namespace std::rel_ops;

class RegKey;
class RegKeyForwardIterator;
class RegValueForwardIterator;

// Variant class for Registry values.
struct RegValue {
	DWORD Type;
	std::vector<BYTE> Data;
	RegValue()
		: Type(REG_NONE), Data(0)
	{ }
	// binary data
	RegValue(DWORD cbData, LPBYTE lpData)
		: Type(REG_BINARY), Data(lpData, lpData + cbData)
	{ }

	bool operator==(const RegValue& v) const
	{
		return Type == v.Type && Data == v.Data;
	}
	// assist overloading
	bool operator!=(const RegValue& v) const
	{
		return !operator==(v);
	}
	bool operator<(const RegValue& v)
	{
		return Type < v.Type || Type == v.Type && Data < v.Data;
	}

	// 4 byte unsigned int
	RegValue(DWORD dwData)
		: Type(REG_DWORD), Data(reinterpret_cast<BYTE*>(&dwData), reinterpret_cast<BYTE*>(&dwData) + sizeof(DWORD))
	{ }
	RegValue& operator=(DWORD dwData)
	{
		Type = REG_DWORD;
		Data.assign(reinterpret_cast<BYTE*>(&dwData), reinterpret_cast<BYTE*>(&dwData) + sizeof(DWORD));

		return *this;
	}
	bool operator==(DWORD dwData) const
	{
		return Type == REG_DWORD 
			&& 0 == memcmp(&dwData, &Data[0], sizeof(DWORD));
	}
	operator DWORD() const
	{
		ensure (Type == REG_DWORD);

		return *(DWORD*)(&Data[0]);
	}
	operator DWORD&()
	{
		ensure (Type == REG_DWORD);

		return *(DWORD*)(&Data[0]);
	}

	// 8 byte unsigned int
	RegValue(QWORD qwData)
		: Type(REG_QWORD), Data(reinterpret_cast<BYTE*>(&qwData), reinterpret_cast<BYTE*>(&qwData) + sizeof(QWORD))
	{ }
	RegValue& operator=(QWORD qwData)
	{
		Type = REG_QWORD;
		Data.assign(reinterpret_cast<BYTE*>(&qwData), reinterpret_cast<BYTE*>(&qwData) + sizeof(QWORD));

		return *this;
	}
	bool operator==(QWORD qwData) const
	{
		return Type == REG_QWORD 
			&& 0 == memcmp(&qwData, &Data[0], sizeof(QWORD));
	}
	operator QWORD() const
	{
		ensure (Type == REG_QWORD);

		return *(QWORD*)(&Data[0]);
	}
	operator QWORD&()
	{
		ensure (Type == REG_QWORD);

		return *(QWORD*)(&Data[0]);
	}

	// null terminated string
	RegValue(LPCTSTR lpData, bool expand = false)
		: Type(expand ? REG_EXPAND_SZ : REG_SZ), Data(lpData, lpData + strlen(lpData) + 1)
	{ 
	
	}

	RegValue& operator=(LPCTSTR lpData)
	{
		Type = REG_SZ;
		Data.assign(lpData, lpData + strlen(lpData) + 1);

		return *this;
	}
	bool operator==(LPCTSTR lpData) const
	{
		return Type == REG_SZ && strlen(lpData) + 1 == Size()
			&& 0 == memcmp(lpData, &Data[0], Size());
	}
	// only const version
	operator LPCTSTR() const
	{
		return (LPCTSTR)&Data[0];
	}

	// double null terminated array of null terminated strings
	RegValue(WORD n, LPCTSTR lpData[])
		: Type(REG_MULTI_SZ)
	{
		for (size_t i = 0; i < n; ++i) {
			Data.insert(Data.end(), lpData[i], lpData[i] + strlen(lpData[i]) + 1);
		}
		Data.insert(Data.end(), 0);
	}
	const LPCTSTR at(size_t i) const
	{
		if (i == 0 && (Type == REG_SZ || Type == REG_EXPAND_SZ))
			return (LPCTSTR)&Data[0];

		ensure (Type == REG_MULTI_SZ);

		LPCTSTR b = (LPCTSTR)&Data[0];
		while (i--) {
			b += strlen(b) + 1;
			if (*b == 0)
				return 0;
		}

		return b;
	}

	// helpers for Data
	DWORD Size(void) const
	{
		return static_cast<DWORD>(Data.size());
	}
	void Resize(DWORD size)
	{
		Data.resize(size);
	}
	LPVOID Pointer(void) const
	{
		return (LPVOID)&Data[0];
	}
};


class RegKey {
	HKEY h0_, h_;
	REGSAM sam_;
	Tstring p_;
public:
	RegKey(HKEY hKey = 0, LPCTSTR machineName = 0)
		: h0_(hKey), h_(hKey), sam_(0), p_()
	{
		LONG ret(ERROR_SUCCESS);

		if (machineName && *machineName && h0_) {
			ret = RegConnectRegistry(machineName, h0_, &h_);
		}
	}
	RegKey(const RegKey& k)
		: h0_(k.h0_), h_(0), sam_(k.sam_), p_(k.p_)
	{
		Open(p_.c_str(), sam_);
	}
	RegKey& operator=(const RegKey& k)
	{
		if (this != &k) {
			h0_ = k.h0_;
			h_ = 0;
			sam_ = k.sam_;
			p_ = k.p_;
			Open(p_.c_str(), sam_);
		}

		return *this;
	}
	~RegKey()
	{
		if (h_ && h0_ != h_)
			RegCloseKey(h_);
	}
	operator HKEY() const
	{
		return h_;
	}
	operator HKEY&()
	{
		return h_;
	}
	HKEY Hive() const
	{
		return h0_;
	}
	HKEY Key() const
	{
		return h0_ == h_ ? 0 : h_;
	}
	REGSAM SAM() const
	{
		return sam_;
	}
	LPCTSTR Path() const
	{
		return &p_[0];
	}
	// key has no subkeys
	bool isLeaf(void) const;

	// key has no values
	bool isEmpty(void) const;

	LONG Open(LPCTSTR subKey, REGSAM samDesired = KEY_ALL_ACCESS)
	{
		LONG ret;

		if (Key())
			RegCloseKey(Key());

		ret = RegOpenKeyEx(Hive(), subKey, 0, samDesired, &h_);
		sam_ = samDesired;
		p_ = subKey;

		return ret;
	}
	LONG Create(LPCTSTR subKey, REGSAM samDesired = KEY_ALL_ACCESS)
	{
		LONG ret;
		
		if (Key()) RegCloseKey(Key());
		ret = RegCreateKeyEx(Hive(), subKey, 0, "", 0, samDesired, 0, &h_, 0);
		sam_ = samDesired;
		p_ = subKey;

		return ret;
	}
	LONG Close()
	{
		LONG ret(ERROR_SUCCESS);

		if (h_) {
			// don't touch the hive
			ret = RegCloseKey(Key());
			h_ = 0;
			sam_ = 0;
			p_ = _T("");
		}

		return ret;
	}

	// recursive delete
	LONG Delete();

	// handle RegValue size issues
	LONG QueryValue(LPCTSTR valueName, RegValue& v) const
	{
		LONG ret;
		DWORD n;

		if (v.Size() == 0) {
			ret = RegQueryValueEx(Key(), valueName, 0, &v.Type, 0, &n);
			if (ret != ERROR_SUCCESS) {
				v = RegValue();
				
				return ret;
			}
			v.Resize(n);
		}

		n = v.Size();
		ret = RegQueryValueEx(h_, valueName, 0, &v.Type, &v.Data[0], &n);

		if (ret == ERROR_MORE_DATA || v.Size() < n) {
			v.Resize(n);
			ret = RegQueryValueEx(h_, valueName, 0, &v.Type, &v.Data[0], &n);
		}

		return ret;
	}
	// proxy for RegValue l-values
	class RegValueProxy {
		RegKey&  k_;
		LPCTSTR  p_;
	public:
		RegValueProxy(RegKey& key, LPCTSTR path)
			: k_(key), p_(path)
		{ }
		RegValueProxy& operator=(const RegValueProxy& p)
		{
			if (this != &p) {
				k_ = p.k_;
				p_ = p.p_;
			}

			return *this;
		}
		RegValueProxy& operator=(const RegValue& v)
		{
			k_.SetValue(p_, v);

			return *this;
		}
		template <class T>
		bool operator==(const T& t) const
		{
			return operator RegValue() == t;
		}
		// for r-value
		operator RegValue() const
		{
			RegValue v;

			k_.QueryValue(p_, v);

			return v;
		}
	};
	// use proxy for get-set value operations
	RegValueProxy operator[](LPCTSTR valueName)
	{
		return RegValueProxy(*this, valueName);
	}
	LONG SetValue(LPCTSTR valueName, const RegValue& Value)
	{
		return RegSetValueEx(h_, valueName, 0, Value.Type, &Value.Data[0], Value.Size());
	}
	LONG EnumKey(DWORD index, LPTSTR name, LPDWORD nameCount)
	{
		return RegEnumKeyEx(h_, index, name, nameCount, 0, 0, 0, 0);
	}
	LONG EnumValue(DWORD index, LPTSTR valueName, LPDWORD valueNameCount, RegValue& Value)
	{
		DWORD n(Value.Size());

		if (n == 0) {
			RegEnumValue(h_, index, valueName, valueNameCount, 0, &Value.Type, 0, &n);
			Value.Resize(n);
		}

		return RegEnumValue(h_, index, valueName, valueNameCount, 0, &Value.Type, &Value.Data[0], &n);
	}
};

// iterate over subkeys
class RegKeyForwardIterator 
	: public std::iterator<std::forward_iterator_tag, TCHAR*>
{
	HKEY k_;
	DWORD i_, max_;
	Tstring n_;
public:
	// end of enumeration value
	RegKeyForwardIterator()
		: k_(0), i_(0), max_(0)
	{ }
	RegKeyForwardIterator(const HKEY& k)
		: k_(k), i_(0), max_(0)
	{
		DWORD n; // subkeys

		// longest subkey name
		RegQueryInfoKey(k, 0, 0, 0, &n, &max_, 0, 0, 0, 0, 0, 0);
		if (n == 0) {
			k_ = 0;	// end of enumeration

			return;
		}
		n_.resize(++max_);
		n = max_;
		RegEnumKeyEx(k_, i_, &n_[0], &n, 0, 0, 0, 0);
		n_.resize(n+1);
	}
	value_type operator*()
	{
		return n_.size() ? &n_[0] : 0;
	}
	RegKeyForwardIterator& operator++()
    {
		LONG ret;
		DWORD n(max_);

		n_.resize(n);
		ret = RegEnumKeyEx(k_, ++i_, &n_[0], &n, 0, 0, 0, 0);
		if (ret != ERROR_SUCCESS) {
			k_ = 0;
			i_ = 0;
			n_.clear();
		}
		else {
			n_.resize(n + 1);
		}

		return *this;
    } 
	bool operator==(const RegKeyForwardIterator& i) const
	{
		return k_ == i.k_ && i_ == i.i_ && n_ == i.n_;
	}
	bool operator!=(const RegKeyForwardIterator& i) const
	{
		return !operator==(i);
	}
};

// iterator over values
class RegValueForwardIterator 
	: public std::iterator<
		std::forward_iterator_tag,
		std::pair<TCHAR*, RegValue>
	>
{
	HKEY k_;
	DWORD i_, nmax_, vmax_;
	Tstring n_;
	RegValue v_;
public:
	// end of enumeration iterator
	RegValueForwardIterator()
		: k_(0), i_(0), nmax_(0), vmax_(0)
	{ }
	RegValueForwardIterator(const HKEY& k)
		: k_(k), i_(0), nmax_(0), vmax_(0)
	{
		DWORD nv; // number of values in key

		// longest value name and data length
		RegQueryInfoKey(k_, 0, 0, 0, 0, 0, 0, &nv, &nmax_, &vmax_, 0, 0);
		if (nv == 0) {
			k_ = 0; // end of enumeration

			return;
		}
		n_.resize(++nmax_);
		v_.Resize(vmax_);

		DWORD n(nmax_), v(vmax_);
		RegEnumValue(k_, i_, &n_[0], &n, 0, &v_.Type, &v_.Data[0], &v);
		n_.resize(n + 1);
		v_.Resize(v);
	}
	value_type operator*()
	{
		return n_.size() 
			? std::make_pair(&n_[0], v_)
			: std::make_pair(static_cast<TCHAR*>(0), v_);
	}
	RegValueForwardIterator& operator++()
    {
		LONG ret;
		DWORD n(nmax_), v(vmax_);

		ensure (k_);
		n_.resize(n);
		v_.Resize(v);
		ret = RegEnumValue(k_, ++i_, &n_[0], &n, 0, &v_.Type, &v_.Data[0], &v);
		if (ret != ERROR_SUCCESS) {
			k_ = 0;
			i_ = 0;
			n_.clear();
			v_.Data.clear();
		}
		else {
			n_.resize(n + 1);
			v_.Resize(v);
		}

		return *this;
    }
	bool operator==(const RegValueForwardIterator& i) const
	{
		return k_ == 0 ? i.k_ == 0 : k_ == i.k_ && i_ == i.i_ && n_ == i.n_ && v_ == i.v_;
	}
};

// key has no subkeys
inline bool
RegKey::isLeaf(void) const
{
	return RegKeyForwardIterator(h_) == RegKeyForwardIterator();
}
// key has no values
inline bool 
RegKey::isEmpty(void) const
{
	return RegValueForwardIterator(h_) == RegValueForwardIterator();
}
// recursively delete key and all subkeys
inline LONG 
RegKey::Delete()
{
	ensure (Hive());
	ensure (Path());

	// have to copy all subkeys
	std::vector<Tstring> subkey;
	RegKeyForwardIterator i(h_), e;
	std::copy(i, e, 
		std::back_insert_iterator<std::vector<Tstring> >(subkey));

	for (size_t i = 0; i < subkey.size(); ++i) {
		RegKey k(Hive());
		k.Open((Tstring(Path()) + "\\" + subkey[i]).c_str());
		k.Delete();
	}

	RegDeleteKey(Hive(), Path());

	return 0;
}
