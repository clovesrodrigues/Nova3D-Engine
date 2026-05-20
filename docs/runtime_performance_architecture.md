# Nova3D Runtime Performance Architecture Foundation

This document defines Nova3D's scalable runtime foundation for multithreaded execution, async resources, streaming, GPU upload staging, diagnostics, and subsystem isolation.

## Core task system
- `Task`, `TaskHandle`, `TaskQueue`, `TaskGraph`, `TaskScheduler` (thread-pool alias) provide backend-independent orchestration.
- Priority queues + dependency checks support async jobs, queued jobs, and synchronization points.
- The design is renderer-agnostic and reusable for animation, streaming, importing, and future physics jobs.

## Threading ownership philosophy
- `ThreadContext` and `ThreadManager` provide classification for main/worker/render-prep/io threads.
- No OpenGL-specific threading contracts are exposed.
- Ownership remains at runtime systems, not backend objects.

## Async resource flow
1. `AsyncResourceRequest` is submitted to `AsyncResourceLoader`.
2. Work is delegated to `TaskScheduler`.
3. Streaming continuation is queued via `ResourceStreamingRequest` / `StreamingManager`.
4. Completion notifications are emitted through event bus.

## GPU upload architecture
- `GPUUploadQueue`, `GPUUploadTask`, `DeferredUploadRequest`, and `UploadStagingBuffer` define deferred GPU transfer staging.
- Public API is backend independent and ready for Vulkan staging buffers or render-thread submission.

## Memory allocator philosophy
- `MemoryTracker` / `MemoryStats` for diagnostics and tagging.
- `MemoryArena`, `LinearAllocator`, `StackAllocator`, `PoolAllocator`, `FrameAllocator` provide specialization-ready foundations.
- STL-wide replacement is intentionally deferred.

## Profiler and diagnostics architecture
- `Profiler`/`CPUProfiler`, `GPUProfiler` abstraction, `ProfileScope`, `TimingStats`.
- `FrameStatistics`, `RenderStatistics`, `StreamingStatistics`, `EngineStatistics`, `RuntimeStats` for runtime panels and telemetry.

## Streaming strategy
- `StreamingManager`, `StreamingRegion`, `StreamingChunk`, `StreamableResource`, `StreamingPriority` establish queue-first streaming preparation.
- Region and chunk semantics are intentionally light to keep future world-streaming designs flexible.

## Render-thread preparation
- `RenderCommand`, `RenderCommandQueue`, `RenderFrameContext`, `RendererSubmissionQueue` allow deferred submission pipelines now.
- Full render-thread split is intentionally postponed.

## Subsystem ownership model
- `EngineSubsystem`, `SubsystemRegistry`, `EngineContext`, `ServiceLocator` avoid singleton sprawl and support deterministic lifecycle ordering.
- Architecture is plugin/editor-ready with explicit ownership and startup/shutdown control.

## Event architecture and hot-reload prep
- `EngineEvent`, `EventQueue`, `EventBus`, `EventDispatcher` provide async-safe subsystem messaging.
- `FileWatcher`, `FileWatchEvent`, `AssetChangeMonitor` define abstraction-ready live-reload inputs.

## Why this scales better than older Irrlicht-era designs
- Explicit async queues and task dependencies replace synchronous update bottlenecks.
- Subsystem registry + service location improve isolation versus global mutable managers.
- Backend-independent upload, profiling, and diagnostics paths keep portability for Vulkan/Android/editor integration.
- Data flow is prepared for multithread scaling while preserving the engine's simplicity goals.
