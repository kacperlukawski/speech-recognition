#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <list>
#include <memory>
#include <vector>
#include <stdexcept>
#include "DataSample.h"

using std::list;
using speech::raw_data::DataSample;

namespace speech {

    namespace raw_data {

        /**
        * This abstract class is a base for all data sources used in the application.
        * By data source we define the source of the audio signal.
        *
        */
        template<typename FrameType>
        class DataSource {
        protected:
            std::shared_ptr<list<DataSample<FrameType>>> samples;

            /** Length of the single sample in milliseconds */
            int sampleLength;

            virtual unsigned int getDataSampleSize(int sizeInMilliseconds) = 0;

        public:

            class iterator {
            private:
                DataSource<FrameType> &_dataSource;
                typename list<DataSample<FrameType>>::iterator samplesIterator;
                int offset;
                int dataSampleSize;
                int offsetInMilliseconds;
                int dataSampleSizeInMilliseconds;

                std::shared_ptr<FrameType> currIterationPtr;
                int currDataSampleSize;
                int currDataSamplePosition;

                std::vector<FrameType> *windowVector;

            public:
                iterator(DataSource<FrameType> &dataSource,
                         typename list<DataSample<FrameType>>::iterator samplesIterator, int offsetInMilliseconds,
                         int dataSampleSizeInMilliseconds) : iterator(dataSource, samplesIterator, offsetInMilliseconds,
                                                                      dataSampleSizeInMilliseconds, false) {
                }

                iterator(DataSource<FrameType> &dataSource,
                         typename list<DataSample<FrameType>>::iterator samplesIterator, int offsetInMilliseconds,
                         int dataSampleSizeInMilliseconds, bool endIterator) : _dataSource(dataSource) {
                    if (offsetInMilliseconds >= dataSampleSizeInMilliseconds) {
                        throw std::invalid_argument("Sample size must be bigger than offset size.");
                    }

                    this->samplesIterator = samplesIterator;
                    this->offsetInMilliseconds = offsetInMilliseconds;
                    this->dataSampleSizeInMilliseconds = dataSampleSizeInMilliseconds;

                    this->offset = _dataSource.getDataSampleSize(offsetInMilliseconds);
                    this->dataSampleSize = _dataSource.getDataSampleSize(dataSampleSizeInMilliseconds);

                    this->windowVector = new std::vector<FrameType>();

                    if (endIterator) {
                        samplesIterator--;
                        this->currDataSamplePosition = samplesIterator->getSize() - 1;
                        this->currIterationPtr = samplesIterator->getValues();
                        this->currDataSampleSize = samplesIterator->getSize();
                        samplesIterator++;
                    } else {
                        this->currIterationPtr = samplesIterator->getValues();
                        this->currDataSampleSize = samplesIterator->getSize();
                        this->currDataSamplePosition = 0;
                        ++(*this);
                    }
                }

                ~iterator() {
                    delete windowVector;
                    currIterationPtr.reset();
                }

                bool operator==(const iterator &it) {
                    throw std::logic_error("DataSample::iterator::operator== not yet implemented.");

                    // TODO
                    return (this->currIterationPtr.get() == it.currIterationPtr.get()
                            && this->currDataSamplePosition == it.currDataSamplePosition);
                }

                bool operator!=(const iterator &it) {
                    return !(*this == it);
                }

                void operator++() {
                    std::vector<FrameType> vectorTmp;
                    if (windowVector->size() > offset) {
                        for (int i = windowVector->size() - offset; i < windowVector->size(); ++i) {
                            vectorTmp.push_back((*windowVector)[i]);
                        }
                    }

                    windowVector->clear();
                    windowVector->insert(windowVector->begin(), vectorTmp.begin(), vectorTmp.end());

                    while (windowVector->size() < dataSampleSize) {
                        for (; currDataSamplePosition < currDataSampleSize; ++currDataSamplePosition) {
                            windowVector->push_back(currIterationPtr.get()[currDataSamplePosition]);
                        }
                        if (currDataSamplePosition >= currDataSampleSize) {
                            ++samplesIterator;
                            currIterationPtr = samplesIterator->getValues();
                            currDataSampleSize = samplesIterator->getSize();
                            currDataSamplePosition = 0;
                        }
                    }
                }

                DataSample<FrameType> operator*() {
                    int windowSize = (*windowVector).size();
                    FrameType *vals = new FrameType[windowSize];
                    for (int i = 0; i < windowSize; ++i) {
                        vals[i] = (*windowVector)[i];
                    }
                    std::shared_ptr<FrameType> windowVectorPtr(vals, std::default_delete<FrameType[]>());
                    return DataSample<FrameType>(dataSampleSize, dataSampleSizeInMilliseconds, windowVectorPtr);
                }
            };

            DataSource(int sampleLength);

            virtual ~DataSource();

            virtual void addSample(DataSample<FrameType> sample);

            virtual typename list<DataSample<FrameType>>::iterator getSamplesIteratorBegin();

            virtual typename list<DataSample<FrameType>>::iterator getSamplesIteratorEnd();

            virtual DataSource<FrameType>::iterator getSamplesOffsetIteratorBegin(int windowSizeInMilliseconds,
                                                                                  int offsetInMilliseconds);

            virtual DataSource<FrameType>::iterator getSamplesOffsetIteratorEnd(int windowSizeInMilliseconds,
                                                                                int offsetInMilliseconds);
        };

    }

}

#endif