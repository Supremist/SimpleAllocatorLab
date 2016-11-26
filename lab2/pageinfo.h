#pragma once
#include <memory>
#include "blockmemoryrange.h"
#include "memorymanager.h"

using ClassifiedManager = MemoryManager<BlockMemoryRange>;
using PagesList = list<ClassifiedManager::MemoryIter>;

class PageInfo
{
public:
	using Ptr = std::shared_ptr<PageInfo>;

	static PageInfo * createLargePage(ClassifiedManager::MemoryIter managed) {
		PageInfo *inst = new PageInfo();
		inst->m_managedIter = managed;
		return inst;
	}

	static PageInfo * createClassifiedPage(PagesList *pages, PagesList::iterator iter) {
		PageInfo *inst = new PageInfo();
		inst->m_equsizedPages = pages;
		inst->m_pageMapIter = iter;
		inst->m_managedIter = *iter;
		return inst;
	}

	ClassifiedManager::MemoryIter managedIter() const {
		return m_managedIter;
	}

	bool isLarge() const {
		return m_equsizedPages == nullptr;
	}

	void free() {
		if (!isLarge() && m_equsizedPages) {
			m_equsizedPages->erase(m_pageMapIter);
		}
		m_isFree = true;
	}

	bool isFree() const {
		return m_isFree;
	}

	~PageInfo() {
		free();
	}

private:
	PageInfo(){}
	bool m_isFree = false;
	ClassifiedManager::MemoryIter m_managedIter;
	PagesList *m_equsizedPages;
	PagesList::iterator m_pageMapIter;

};
