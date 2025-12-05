/*
 * =====================================================================================
 *
 *       Filename:  Mark.h
 *    Description:
 *        Created:  08/01/2007 03:52:25 PM
 *         Author:  Daniel f. Gomez-Prado (dgomezpr), dgomezpr@ecs.umass.edu
 *        Company:  UMASS
 *
 * =====================================================================================
 *  $Revision:: 178                                          $: Revision of last commit
 *  $Author:: daniel                                         $: Author of last commit
 *  $Date:: 2012-01-12 18:55:36 -0500 (Thu, 12 Jan 2012)     $: Date of last commit
 * =====================================================================================
 */

#ifndef __H_traversal_mark
#define __H_traversal_mark

namespace dtl {

	/**
	 * @class Mark
	 * @brief Provides a mechanism to mark nodes as visited
	 * @details This is a template class, just because the traversalMark needs to be static so every element of class T
	 * has a global static mark as a reference. With this approach we can have different classes T with an element Mark<T>
	 * with each T having its own Mark::currentmark
	 **/
	template <typename T>
		class Mark {
		private:
			static int traversalMark;
			int _traversal_mark;
			static int propertyMark;
			int _property_mark;
		public:
			static void newMark(void);
			static void resetMark(void);
			void setMark (void);
			void cleanMark(void);
			bool isMarked(void) const;
			void setWaterMark (void);
			bool isWaterMarked(void) const;
			int getMark(void) const;

			static void newPMark(void);
			static void resetPMark(void);
			void cleanPMark(void);
			void setPMark(void);
			bool isPMarked(void) const;
			/** @brief Creates a new instance of Mark*/
			Mark(void): _traversal_mark(0), _property_mark(0) {}
			~Mark(void) {}
		};

	/** @brief Current value of the mark for class T*/
	template<typename T>
		int Mark<T>::traversalMark = 1;

	template<typename T>
		int Mark<T>::propertyMark = 1;

	// INLINE FUNCTIONS
	/** @brief Starts a new mark; all other marks for class T become invalid.*/
	template<typename T>
		inline void Mark<T>::newMark(void) { traversalMark += 2; }
	template<typename T>
		inline void Mark<T>::resetMark(void) { traversalMark = 1; }
	template<typename T>
		inline void Mark<T>::setWaterMark(void) { _traversal_mark = traversalMark-1; }
	template<typename T>
		inline void Mark<T>::setMark(void) { _traversal_mark = traversalMark; }
	/** @brief Cleans the mark.*/
	template<typename T>
		inline void Mark<T>::cleanMark(void) { _traversal_mark = 1; }
	template<typename T>
		inline bool Mark<T>::isMarked (void) const { return (_traversal_mark == traversalMark); }
	template<typename T>
		inline bool Mark<T>::isWaterMarked(void) const { return (_traversal_mark == traversalMark-1); }
	template<typename T>
		inline int Mark<T>::getMark(void) const { return _traversal_mark; }

	template<typename T>
		inline void Mark<T>::newPMark(void) { propertyMark += 2; }
	template<typename T>
		inline void Mark<T>::resetPMark(void) { propertyMark = 1; }
	template<typename T>
		inline void Mark<T>::cleanPMark(void) { _property_mark = 1; }
	template<typename T>
		inline void Mark<T>::setPMark(void) { _property_mark = propertyMark; }
	template<typename T>
		inline bool Mark<T>::isPMarked (void) const { return (_property_mark == propertyMark); }

}

#endif
