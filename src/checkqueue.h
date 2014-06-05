


#ifndef CHECKQUEUE_H
#define CHECKQUEUE_H

#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/condition_variable.hpp>

#include <vector>
#include <algorithm>

template<typename T> class CCheckQueueControl;


template<typename T> class CCheckQueue {
private:

    boost::mutex mutex;


    boost::condition_variable condWorker;


    boost::condition_variable condMaster;



    std::vector<T> queue;


    int nIdle;


    int nTotal;


    bool fAllOk;




    unsigned int nTodo;


    bool fQuit;


    unsigned int nBatchSize;


    bool Loop(bool fMaster = false) {
        boost::condition_variable &cond = fMaster ? condMaster : condWorker;
        std::vector<T> vChecks;
        vChecks.reserve(nBatchSize);
        unsigned int nNow = 0;
        bool fOk = true;
        do {
            {
                boost::unique_lock<boost::mutex> lock(mutex);

                if (nNow) {
                    fAllOk &= fOk;
                    nTodo -= nNow;
                    if (nTodo == 0 && !fMaster)

                        condMaster.notify_one();
                } else {

                    nTotal++;
                }

                while (queue.empty()) {
                    if ((fMaster || fQuit) && nTodo == 0) {
                        nTotal--;
                        bool fRet = fAllOk;

                        if (fMaster)
                            fAllOk = true;

                        return fRet;
                    }
                    nIdle++;

                    nIdle--;
                }





                nNow = std::max(1U, std::min(nBatchSize, (unsigned int)queue.size() / (nTotal + nIdle + 1)));
                vChecks.resize(nNow);
                for (unsigned int i = 0; i < nNow; i++) {


                     vChecks[i].swap(queue.back());
                     queue.pop_back();
                }

                fOk = fAllOk;
            }

            BOOST_FOREACH(T &check, vChecks)
                if (fOk)
                    fOk = check();
            vChecks.clear();
        } while(true);
    }

public:

    CCheckQueue(unsigned int nBatchSizeIn) :
        nIdle(0), nTotal(0), fAllOk(true), nTodo(0), fQuit(false), nBatchSize(nBatchSizeIn) {}


    void Thread() {
        Loop();
    }


    bool Wait() {
        return Loop(true);
    }


    void Add(std::vector<T> &vChecks) {
        boost::unique_lock<boost::mutex> lock(mutex);
        BOOST_FOREACH(T &check, vChecks) {
            queue.push_back(T());
            check.swap(queue.back());
        }
        nTodo += vChecks.size();
        if (vChecks.size() == 1)
            condWorker.notify_one();
        else if (vChecks.size() > 1)
            condWorker.notify_all();
    }

    ~CCheckQueue() {
    }

    friend class CCheckQueueControl<T>;
};


template<typename T> class CCheckQueueControl {
private:
    CCheckQueue<T> *pqueue;
    bool fDone;

public:
    CCheckQueueControl(CCheckQueue<T> *pqueueIn) : pqueue(pqueueIn), fDone(false) {

        if (pqueue != NULL) {
            assert(pqueue->nTotal == pqueue->nIdle);
            assert(pqueue->nTodo == 0);
            assert(pqueue->fAllOk == true);
        }
    }

    bool Wait() {
        if (pqueue == NULL)
            return true;
        bool fRet = pqueue->Wait();
        fDone = true;
        return fRet;
    }

    void Add(std::vector<T> &vChecks) {
        if (pqueue != NULL)
            pqueue->Add(vChecks);
    }

    ~CCheckQueueControl() {
        if (!fDone)
            Wait();
    }
};

#endif
