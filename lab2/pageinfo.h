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
		return m_managedIter->isLarge();
	}

	void free() {
		if (!isLarge() && !isFree() && m_equsizedPages) {
			m_pageMapIter = m_equsizedPages->erase(m_pageMapIter);
		}
		m_isFree = true;
	}

	void restore(const ClassifiedManager::MemoryIter &newManaged) {
		m_managedIter = newManaged;
		if (!isLarge() && isFree() && m_equsizedPages) {
			m_pageMapIter = m_equsizedPages->insert(m_pageMapIter, m_managedIter);
		}
		m_isFree = false;

	}

	bool isFree() const {
		return m_isFree;
	}

	~PageInfo() {
		free();
	}

	PageInfo & operator= (const PageInfo &other) = delete;

private:
	PageInfo(){}
	bool m_isFree = false;
	ClassifiedManager::MemoryIter m_managedIter;
	PagesList *m_equsizedPages = nullptr;
	PagesList::iterator m_pageMapIter;
};
