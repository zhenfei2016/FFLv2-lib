
#include <pipeline/FFL_NodeBase.hpp>

namespace FFL {	

		InputInterface::InputInterface() :
			mNodeId(FFL::Pipeline_INVALID_Id),
			mId(FFL::Pipeline_INVALID_Id) {
		}
		InputInterface::InputInterface(FFL::PipelineNodeId nodeId, FFL::PipelineInputId inputId) :
			mNodeId(nodeId), mId(inputId)
		{
		}
		void InputInterface::reset() {
			mNodeId = FFL::Pipeline_INVALID_Id;
			mId = FFL::Pipeline_INVALID_Id;
		}
		bool InputInterface::isValid() const {
			return mNodeId != FFL::Pipeline_INVALID_Id && mId != FFL::Pipeline_INVALID_Id;
		}
		//
		// 获取所属的Nodeid和接口的id
		//
		FFL::PipelineNodeId InputInterface::getNodeId() const {
			return mNodeId;
		}
		FFL::PipelineNodeId InputInterface::getId() const {
			return mId;
		}
		const char* InputInterface::getName() {
			return mName.c_str();
		}
		OutputInterface::OutputInterface() :
			mNodeId(FFL::Pipeline_INVALID_Id),
			mId(FFL::Pipeline_INVALID_Id) {
		}
		OutputInterface::OutputInterface(FFL::PipelineNodeId nodeId, FFL::PipelineOutputId outputId) :
			mNodeId(nodeId), mId(outputId)
		{
		}
		void OutputInterface::reset() {
			mNodeId = FFL::Pipeline_INVALID_Id;
			mId = FFL::Pipeline_INVALID_Id;
		}
		bool OutputInterface::isValid() const {
			return mNodeId != FFL::Pipeline_INVALID_Id && mId != FFL::Pipeline_INVALID_Id;
		}
		//
		// 获取所属的Nodeid和接口的id
		//
		FFL::PipelineNodeId OutputInterface::getNodeId() const {
			return mNodeId;
		}
		FFL::PipelineNodeId OutputInterface::getId() const {
			return mId;
		}
		///////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////
		NodeManager::NodeManager(FFL::sp<FFL::Pipeline> pipeline) {
			mPipeline = pipeline;
		}
		NodeManager::~NodeManager() {

		}
		void NodeManager::registerNode(FFL::sp<NodeBase> node) {
			FFL::CMutex::Autolock l(mNodeLock);
			mNodeList.push_back(node);
		}
		void NodeManager::unRegisterNode(FFL::sp<NodeBase> node) {
			FFL::CMutex::Autolock l(mNodeLock);
			for (FFL::List<  FFL::sp<NodeBase> >::iterator it = mNodeList.begin();
				it != mNodeList.end(); it++) {
				if (it->get() == node.get()) {
					mNodeList.erase(it);
					break;
				}
			}
		}
		FFL::Pipeline* NodeManager::getPipeline() {
			return mPipeline.get();
		}
		//////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////
		class NodeBase::NodeHandler : public FFL::PipelineNodeHandler {
		public:
			NodeHandler(NodeBase* node) :mNodeBase(node) {
			}
			~NodeHandler() {
			}
			//
			//  返回false则node不进行启动了
			//
			bool onStartupBefore() {
				FFL_LOG_INFO("%s: onStartupBefore %p", mNodeBase->mNodeName.c_str(), this);
				return true;
			}
			void onStartupAfter() {
				FFL_LOG_INFO("%s: onStartupAfter %p", mNodeBase->mNodeName.c_str(), this);
			}

			void onShutdownBefore() {
				FFL_LOG_INFO("%s: onShutdownBefore %p", mNodeBase->mNodeName.c_str(), this);
			}
			void onShutdownAfter() {
				FFL_LOG_INFO("%s: onShutdownAfter %p", mNodeBase->mNodeName.c_str(), this);
			}

			void onStartMessageLooper(FFL::PipelineInputId id) {
				FFL_LOG_INFO("%s:%d: onStartMessageLooper %p", mNodeBase->mNodeName.c_str(), id, this);
				mNodeBase->onStartMessageLooper();
			}
			bool prepareHandleMessage(FFL::PipelineInputId id, const FFL::sp<FFL::PipelineMessage> &msg)
			{				
				return true;
			}
			//
			//  退出looper的消息
			//
			void onQuitMessageLooper(FFL::PipelineInputId id, uint32_t exitCode) {
				FFL_LOG_INFO("%s:%d: onQuitMessageLooper %p", mNodeBase->mNodeName.c_str(), id, this);
				mNodeBase->onQuitMessageLooper();
			}

			void onEvent(const FFL::sp<FFL::PipelineEvent> &msg) {
				FFL_LOG_INFO("%s: onEvent %p", mNodeBase->mNodeName.c_str(), this);
			}
		private:
			NodeBase* mNodeBase;
		};

		NodeBase::NodeBase() : mNodeId(FFL::Pipeline_INVALID_Id)
		{
			mNodeMgr = NULL;
			mNodeHandler = new NodeHandler(this);
		}

		NodeBase::~NodeBase()
		{}
		//
		// 开始创建这个节点
		//
		status_t NodeBase::create(NodeManager* mgr) {
			if (mNodeMgr != NULL || mgr == NULL) {
				FFL_LOG_WARNING("NodeBase::create fail ");
				return FFL_ERROR_FAIL;
			}
			mNodeMgr = mgr;

			FFL::sp<FFL::Pipeline> pipeline = mNodeMgr->getPipeline();
			mNodeId = pipeline->createNode(mNodeHandler);
			if (mNodeId == FFL::Pipeline_INVALID_Id) {
				FFL_LOG_ERROR("invalid node id");
			}
			FFL_LOG_INFO("create \"%s\" nodeid=%d", mNodeName.c_str(), getNodeId());
			mNodeMgr->registerNode(this);
			onCreate();
			return FFL_OK;
		}
		void NodeBase::destroy() {
			if (isCreated()) {
				mNodeMgr->unRegisterNode(this);
				mNodeMgr = NULL;
				onDestroy();
			}
		}
		bool NodeBase::isCreated() const {
			return mNodeMgr != NULL;
		}
		void NodeBase::setName(const char* name)
		{
			mNodeName = name ? name : "";
		}
		void NodeBase::getName(FFL::String& name) const {
			name = mNodeName;
		}
		const char* NodeBase::getName() {
			return mNodeName.c_str();
		}
		//
		//  获取这个node的所有者
		//
		NodeManager* NodeBase::getManager() const {
			return mNodeMgr;
		}
		//
		//  获取这个节点属于哪一个线的
		//
		FFL::sp<FFL::Pipeline> NodeBase::getPipeline() {
			return mNodeMgr->getPipeline();
		}
		//
		//  获取nodeid
		//
		FFL::PipelineNodeId NodeBase::getNodeId() const {
			return mNodeId;
		}		
		//
		//  获取输入
		//
		FFL::sp<FFL::PipelineInput> NodeBase::getInput(FFL::PipelineInputId id) {
			return mNodeHandler->getInput(id);
		}	

		FFL::sp<FFL::PipelineConnector> NodeBase::getConnector(FFL::PipelineInputId id) {
			FFL_ASSERT(0);
			return NULL;
		}
		//
		//  创建输入接口
		//
		InputInterface NodeBase::createInputInterface(FFL::sp<FFL::PipelineInputHandler> callback, const char* name) {
			InputInterface input(getNodeId(), FFL::Pipeline_INVALID_Id);
			if (isCreated()) {
				FFL::sp<FFL::Pipeline> pipeline = mNodeMgr->getPipeline();
				input.mId = pipeline->createInput(getNodeId(), callback, name);
				if (!input.isValid()) {
					FFL_LOG_ERROR("invalid input id");
				}
			}
			else {
				FFL_LOG_WARNING("failed to NodeBase::createInputInterface. Node not create. ");
			}
			return input;
		}
		//
		//  output:数据提供者输出接口
		//	input：返回当前的这个节点的输入接口接口
		//  返回是否成功
		//
		InputInterface  NodeBase::connectInput(
			const OutputInterface& output,
			const char* name,
			void* userdata) {
			FFL::sp<FFL::PipelineInputHandler> handler =
				new FFL::ClassMethodInputHandler<NodeBase, void* >(this, &NodeBase::receiveData, userdata);
			InputInterface input = createInputInterface(handler, name);
			FFL::sp<FFL::PipelineConnector > conn = onCreateConnector(output, input, userdata);
			if (connect(output, input, conn)) {
				return input;
			} else {
				FFL_LOG_WARNING("(%s)NodeBase::connectSource failed", mNodeName.c_str());
			}

			input.reset();
			return input;
		}
		InputInterface  NodeBase::connectTimerInput(
			const char* name,			
			uint32_t timeMs, void* userdata) {
			FFL::sp<FFL::PipelineInputHandler> callback =
				new FFL::ClassMethodInputHandler<NodeBase, void*>(this, &NodeBase::receiveData, userdata);
			InputInterface input = createInputInterface(callback, name);
			if (!input.isValid()) {
				FFL_LOG_ERROR("invalid input id");
				input.reset();
				return input;
			}

			FFL::sp<FFL::PipelineSourceConnector> conn = 0;
			if (timeMs==0) {
				conn = new FFL::PipelineIdleSourceConnector();
			}
			else {
				conn = new FFL::PipelineTimerSourceConnector(timeMs);
			}
			getPipeline()->connectSource(input.getNodeId(), input.getId(), conn);
			return input;
		}

		FFL::sp<FFL::PipelineOutput> NodeBase::getOutput(FFL::PipelineOutputId id) {
			return mNodeHandler->getOutput(id);
		}
		//FFL::sp<FFL::PipelineConnector> NodeBase::getConnector(PipelineOutputId id) {
		//	FFL_ASSERT(0);
		//	return NULL;
		//}
		//
		//   创建输出接口
		//
		OutputInterface NodeBase::createOutputInterface() {
			OutputInterface output(getNodeId(), FFL::Pipeline_INVALID_Id);
			if (isCreated()) {
				FFL::sp<FFL::Pipeline> pipeline = mNodeMgr->getPipeline();
				output.mId = pipeline->createOutput(getNodeId());
				if (!output.isValid()) {
					FFL_LOG_ERROR("invalid input id");
				}
			}
			else {
				FFL_LOG_WARNING("failed to NodeBase::createOutputInterface. Node not create. ");
			}
			return output;
		}		
		//
		//   清空输出节点中没有处理的消息
		//
		void NodeBase::clearMessage(FFL::PipelineInputId dstId) {
			FFL::sp<FFL::PipelineOutput > output = getOutput(dstId);
			if (!output.isEmpty()) {
				output->clearMessage();
			}
			else {
				FFL_LOG_WARNING("NodeBase(%s)::clearMessage not find dstid=%d ", dstId);
			}
		}

		//
		//  跟next的node连接到一起，当前node生产的数据，输入到next的name接口中
		//  返回当前的节点的输出接口
		//
		OutputInterface NodeBase::connectNext(NodeBase* next, const char* name, void* userdata) {
			OutputInterface output = createOutputInterface();
			if (output.isValid()) {
				InputInterface input=next->connectInput(output, name, userdata);
			}			
			return output;
		}
	
		//
		//  连接一个输出到一个输入
		//
		bool NodeBase::connect(const OutputInterface& output, const InputInterface& input,
			FFL::sp<FFL::PipelineConnector> conn) {
			if (conn.isEmpty()) {
				return false;
			}

			if (mNodeMgr->getPipeline()->connect(output.mNodeId, output.mId,
				input.mNodeId, input.mId, conn) == FFL_OK) {
				return true;
			}
			return false;
		}
		//
		// 收到消息 ,connectSource建立的连接，连接上的数据来了会触发这个函数
		//
		void NodeBase::receiveData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) {
			onReceivedData(msg, userdata);
		}
		/////////////////////////////////////////////////////////////////////////////////		
		//
		//  发送事件消息
		//
		void NodeBase::postEvent(const FFL::sp<FFL::PipelineEvent> &msg) {
			mNodeHandler->postEvent(msg);
		}
		//
		//  发送消息到output		
		//  dstId: 目标id
		//  msg : 消息内容
		//
		status_t NodeBase::postMessage(FFL::PipelineOutputId dstId, const FFL::sp<FFL::PipelineMessage> &msg) {
			return mNodeHandler->postMessage(dstId, msg, 0);
		}
		status_t NodeBase::postMessageDelay(FFL::PipelineOutputId dstId, const FFL::sp<FFL::PipelineMessage> &msg, uint64_t delayUs = 0) {
			return mNodeHandler->postMessage(dstId, msg, delayUs);
		}
		//
		//  给当前节点的一个输入接口发送一条消息
		//
		status_t NodeBase::postSelfMessage(FFL::PipelineInputId dstId, const FFL::sp<FFL::PipelineMessage> &msg, uint64_t delayUs) {
			return mNodeHandler->postSelfMessage(dstId, msg, delayUs);
		}
	
		/////////////////////////////////////////////////////////////////////////////////
		//
		//   外部setDataInput时候调用此函数，创建对应conn
		//
		FFL::sp<FFL::PipelineConnector > NodeBase::onCreateConnector(
			const OutputInterface& output,
			const InputInterface& input,
			void* userdata) {
			//return new FFL::SyncPipelineConnector();
			return new FFL::PipelineAsyncConnector();
		}
		void NodeBase::onCreate() {}
		void NodeBase::onDestroy() {}
		void NodeBase::onStartMessageLooper()
		{}
		void NodeBase::onQuitMessageLooper()
		{}	
		bool NodeBase::onReceivedData(const FFL::sp<FFL::PipelineMessage>& msg, void* userdata) {
			return false;
		}
	
}
