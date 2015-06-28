#pragma once

namespace glv
{
	template <typename T>
	class NTreeNode :public std::enable_shared_from_this<NTreeNode<T>>
	{
	public:
		typedef NTreeNode<T> Node;
		typedef std::weak_ptr<Node> NodeWeakPtr;
		typedef std::shared_ptr<Node> NodeSharedPtr;
		typedef std::set<NodeSharedPtr> NodeContainer;
		typedef typename NodeContainer::iterator NodeIterator;

	public:
		NTreeNode(T & data)
		{
			mData = data;
		}
		virtual ~NTreeNode()
		{
			//cout << "destructor" << endl;
		}

		const T & data()const
		{
			return mData;
		}
		T data()
		{
			return mData;
		}

		NodeIterator begin()
		{
			return mChildren.begin();
		}
		NodeIterator end()
		{
			return mChildren.end();
		}

		const NodeIterator begin()const
		{
			return mChildren.begin();
		}
		const NodeIterator end()const
		{
			return mChildren.end();
		}

		void addChild(const NodeSharedPtr & nodeRef)
		{
			setParent(shared_from_this(), nodeRef);
			mChildren.insert(nodeRef);
		}
		void removeChild(const NodeSharedPtr & nodeRef)
		{
			mChildren.erase(nodeRef);
			removeParent(nodeRef);
		}

		NodeSharedPtr parent()
		{
			return mParent.lock();
		}

		template <typename Predicate>
		void traverse(const Predicate & predicate)
		{
			const NodeSharedPtr & self = shared_from_this();

			predicate(self);

			for (auto it = self->begin(); it != self->end(); it++)
			{
				if (*it)
					(*it)->traverse(predicate);
			}
		}

	private:
		friend void setParent(NodeSharedPtr parent, NodeSharedPtr child)
		{
			if (parent->mParent.lock() == child)
				throw std::exception("Parent can not be added as child.");
			child->mParent = NodeWeakPtr(parent);
		}

		friend void removeParent(NodeSharedPtr child)
		{
			child->mParent.reset();
		}

	private:
		T mData;

		NodeContainer mChildren;
		NodeWeakPtr mParent;
	};
}


